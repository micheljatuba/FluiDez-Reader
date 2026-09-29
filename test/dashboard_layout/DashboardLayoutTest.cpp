#include <FontCacheManager.h>
#include <GfxRenderer.h>
#include <builtinFonts/all.h>
#include <components/FluiDezBrand.h>
#include <components/icons/book24.h>
#include <components/icons/streak.h>
#include <components/themes/dashboard/DashboardLayout.h>
#include <components/themes/lyra/LyraGridLayout.h>
#include <fontIds.h>
#include <gtest/gtest.h>
#include <images/FluiDezLogo.h>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace {
EpdFont smallFont(&inter_8_regular);
EpdFontFamily smallFontFamily(&smallFont);
const EpdFont uiSymbols10Font(&ui_symbols_10);
EpdFont ui10RegularFont(&inter_10_regular);
EpdFont ui10BoldFont(&inter_10_bold);
EpdFontFamily ui10FontFamily(&ui10RegularFont, &ui10BoldFont, nullptr, nullptr, &uiSymbols10Font);
EpdFont ui12RegularFont(&inter_12_regular);
EpdFont ui12BoldFont(&inter_12_bold);
EpdFontFamily ui12FontFamily(&ui12RegularFont, &ui12BoldFont, nullptr, nullptr, &uiSymbols10Font);

struct RendererFixture {
  HalDisplay display;
  GfxRenderer renderer;
  FontCacheManager cache;

  RendererFixture(const int logicalW, const int logicalH)
      : display(std::max(logicalW, logicalH), std::min(logicalW, logicalH)),
        renderer(display),
        cache(renderer.getFontMap(), renderer.getSdCardFonts()) {
    renderer.begin();
    renderer.setOrientation(logicalW >= logicalH ? GfxRenderer::LandscapeCounterClockwise : GfxRenderer::Portrait);
    renderer.insertFont(UI_10_FONT_ID, ui10FontFamily);
    renderer.insertFont(UI_12_FONT_ID, ui12FontFamily);
    renderer.insertFont(SMALL_FONT_ID, smallFontFamily);
    renderer.setFontCacheManager(&cache);
  }
};

bool intersects(const Rect& a, const Rect& b) {
  return a.x < b.x + b.width && a.x + a.width > b.x && a.y < b.y + b.height && a.y + a.height > b.y;
}

void expectInside(const Rect& r, const int w, const int h) {
  EXPECT_GE(r.x, 0);
  EXPECT_GE(r.y, 0);
  EXPECT_GE(r.width, 0);
  EXPECT_GE(r.height, 0);
  EXPECT_LE(r.x + r.width, w);
  EXPECT_LE(r.y + r.height, h);
}

void expectLineFits(const GfxRenderer& renderer, const DashboardLayout::TextLine& line) {
  const int width =
      renderer.getTextWidth(line.fontId, line.text.c_str(), line.bold ? EpdFontFamily::BOLD : EpdFontFamily::REGULAR);
  EXPECT_LE(width, line.rect.width) << line.text;
}

bool endsWithEllipsis(const std::string& text) {
  constexpr const char* ellipsis = "\xE2\x80\xA6";
  return text.size() >= 3 && text.compare(text.size() - 3, 3, ellipsis) == 0;
}

void expectNoTruncatedLabels(const DashboardLayout::Result& layout) {
  for (const auto& row : layout.stats) {
    for (const auto& label : row.labels) {
      EXPECT_FALSE(endsWithEllipsis(label.text)) << label.text;
    }
  }
  for (const auto& item : layout.footerIcons) {
    EXPECT_FALSE(endsWithEllipsis(item.label.text)) << item.label.text;
  }
  for (const auto& item : layout.footerStats) {
    EXPECT_FALSE(endsWithEllipsis(item.label.text)) << item.label.text;
  }
}

DashboardLayout::Content ptBrNormalContent() {
  return DashboardLayout::Content{"A Sociedade do Anel",
                                  "Capítulo 1: Uma festa muito esperada",
                                  {{"12h 34m", "Leitura"},
                                   {"3h 20m", "Restante"},
                                   {"57%", "Progresso"},
                                   {"15 dias",
                                    "Desde Sep\xC2\xA0"
                                    "22"},
                                   {"Oct 02", "Previsão"},
                                   {"50 min", "Média diária"},
                                   {"1.4", "Páginas/min"}},
                                  {{StreakIcon, "12 dias seguidos"}, {Book24Icon, "Leitor noturno"}},
                                  {}};
}

DashboardLayout::Content enNormalContent() {
  return DashboardLayout::Content{"The Fellowship of the Ring",
                                  "Chapter 1: A Long-expected Party",
                                  {{"12h 34m", "Reading"},
                                   {"3h 20m", "Time Left"},
                                   {"57%", "Progress"},
                                   {"15 days",
                                    "Since Sep\xC2\xA0"
                                    "22"},
                                   {"Oct 02", "Est. Finish"},
                                   {"50 min", "Daily Avg"},
                                   {"1.4", "Pages/Min"}},
                                  {{StreakIcon, "12 day streak"}, {Book24Icon, "Night Reader"}},
                                  {}};
}

DashboardLayout::Content ptBrNoRtcContent() {
  return DashboardLayout::Content{"A Sociedade do Anel",
                                  "Capítulo 1: Uma festa muito esperada",
                                  {{"12h 34m", "Leitura"},
                                   {"3h 20m", "Restante"},
                                   {"57%", "Progresso"},
                                   {"12", "Sessões"},
                                   {"50 min", "Sessão média"},
                                   {"1.4", "Páginas/min"}},
                                  {},
                                  {{"124h", "Tempo total"}, {"7", "Concluídos"}}};
}

DashboardLayout::Content worstContent() {
  return DashboardLayout::Content{
      "Um título de livro deliberadamente muito longo para provar que o Painel nunca ultrapassa os limites da tela",
      "Capítulo extraordinariamente longo com subtítulo e detalhes adicionais que antes poderiam empurrar o rodapé",
      {{"123h 59m", "Previsão estimada extremamente longa"},
       {"123h 59m", "Leitura acumulada com detalhe longo"},
       {"100%", "Progresso da leitura"},
       {"999 dias", "Desde uma data bastante longa"},
       {"Dec 31", "Previsão estimada extremamente longa"},
       {"123h 59m", "Média diária de leitura"},
       {"999.9", "Páginas por minuto / Páginas/min"}},
      {{StreakIcon, "sequência de 999 dias"}, {Book24Icon, "Leitor noturno muito dedicado"}},
      {}};
}

void assertLayout(const GfxRenderer& renderer, const DashboardLayout::Result& layout, const bool hasTouch,
                  const int hintReserve) {
  const int w = renderer.getScreenWidth();
  const int h = renderer.getScreenHeight();
  expectInside(layout.coverRect, w, h);
  expectInside(layout.statsRect, w, h);
  expectInside(layout.textRect, w, h);
  expectInside(layout.footerRect, w, h);
  EXPECT_FALSE(intersects(layout.coverRect, layout.statsRect));
  EXPECT_FALSE(intersects(layout.coverRect, layout.textRect));
  EXPECT_FALSE(intersects(layout.coverRect, layout.footerRect));
  EXPECT_FALSE(intersects(layout.statsRect, layout.textRect));
  EXPECT_FALSE(intersects(layout.statsRect, layout.footerRect));
  EXPECT_FALSE(intersects(layout.textRect, layout.footerRect));
  if (!hasTouch) {
    EXPECT_LE(layout.footerRect.y + layout.footerRect.height, h - hintReserve);
  }
  for (const auto& row : layout.stats) {
    expectInside(row.value.rect, w, h);
    expectLineFits(renderer, row.value);
    EXPECT_GE(row.value.rect.x, layout.statsRect.x);
    EXPECT_LE(row.value.rect.x + row.value.rect.width, layout.statsRect.x + layout.statsRect.width);
    for (const auto& label : row.labels) {
      expectInside(label.rect, w, h);
      expectLineFits(renderer, label);
      EXPECT_GE(label.rect.x, layout.statsRect.x);
      EXPECT_LE(label.rect.x + label.rect.width, layout.statsRect.x + layout.statsRect.width);
    }
  }
  for (const auto& line : layout.titleLines) {
    expectInside(line.rect, w, h);
    expectLineFits(renderer, line);
  }
  for (const auto& line : layout.subtitleLines) {
    expectInside(line.rect, w, h);
    expectLineFits(renderer, line);
  }
  for (const auto& item : layout.footerIcons) {
    expectInside(item.iconRect, w, h);
    expectInside(item.label.rect, w, h);
    expectLineFits(renderer, item.label);
  }
  for (const auto& item : layout.footerStats) {
    expectInside(item.value.rect, w, h);
    expectLineFits(renderer, item.value);
    expectInside(item.label.rect, w, h);
    expectLineFits(renderer, item.label);
  }
}

void writePgm(const std::filesystem::path& path, const GfxRenderer& renderer) {
  std::ofstream out(path, std::ios::binary);
  out << "P5\n" << renderer.getScreenWidth() << " " << renderer.getScreenHeight() << "\n255\n";
  for (int y = 0; y < renderer.getScreenHeight(); ++y) {
    for (int x = 0; x < renderer.getScreenWidth(); ++x) {
      const bool black = renderer.isPixelBlack(x, y);
      const unsigned char pixel = black ? 0 : 255;
      out.write(reinterpret_cast<const char*>(&pixel), 1);
    }
  }
}

void writePreview(const char* name, RendererFixture& fixture, const DashboardLayout::Result& layout) {
  const char* dir = std::getenv("DASHBOARD_PREVIEW_DIR");
  if (dir == nullptr || dir[0] == '\0') {
    return;
  }
  std::filesystem::create_directories(dir);
  fixture.renderer.clearScreen();
  fixture.renderer.fillRectDither(layout.coverRect.x, layout.coverRect.y, layout.coverRect.width,
                                  layout.coverRect.height, Color::LightGray);
  fixture.renderer.drawRect(layout.coverRect.x, layout.coverRect.y, layout.coverRect.width, layout.coverRect.height);
  DashboardLayout::draw(fixture.renderer, layout);
  const auto pgm = std::filesystem::path(dir) / (std::string(name) + ".pgm");
  writePgm(pgm, fixture.renderer);
}

void writeBeforePreview(RendererFixture& fixture) {
  const char* dir = std::getenv("DASHBOARD_PREVIEW_DIR");
  if (dir == nullptr || dir[0] == '\0') {
    return;
  }
  fixture.renderer.clearScreen();
  const Rect cover{20, 70, 296, 444};
  fixture.renderer.fillRectDither(cover.x, cover.y, cover.width, cover.height, Color::LightGray);
  fixture.renderer.drawRect(cover.x, cover.y, cover.width, cover.height);
  const auto content = worstContent();
  const int rightX = 460;
  int y = cover.y;
  for (const auto& row : content.stats) {
    const int valueW = fixture.renderer.getTextWidth(UI_12_FONT_ID, row.value.c_str(), EpdFontFamily::BOLD);
    fixture.renderer.drawText(UI_12_FONT_ID, rightX - valueW, y, row.value.c_str(), true, EpdFontFamily::BOLD);
    const int labelW = fixture.renderer.getTextWidth(SMALL_FONT_ID, row.label.c_str());
    fixture.renderer.drawText(SMALL_FONT_ID, rightX - labelW, y + fixture.renderer.getLineHeight(UI_12_FONT_ID) + 1,
                              row.label.c_str());
    y += 74;
  }
  fixture.renderer.drawText(UI_12_FONT_ID, cover.x, cover.y + cover.height + 28, content.title.c_str(), true,
                            EpdFontFamily::BOLD);
  writePgm(std::filesystem::path(dir) / "before.pgm", fixture.renderer);
}

void convertPreviewsToPng() {
  const char* dir = std::getenv("DASHBOARD_PREVIEW_DIR");
  if (dir == nullptr || dir[0] == '\0') {
    return;
  }
  const auto script = std::filesystem::path(dir) / "pgm_to_png.py";
  std::ofstream out(script);
  out << R"PY(
import pathlib, struct, zlib
root = pathlib.Path(__file__).parent
def read_pgm(path):
    data = path.read_bytes()
    parts = data.split(b'\n', 3)
    w, h = map(int, parts[1].split())
    return w, h, parts[3]
def chunk(t, d):
    return struct.pack(">I", len(d)) + t + d + struct.pack(">I", zlib.crc32(t + d) & 0xffffffff)
for pgm in root.glob("*.pgm"):
    w, h, pixels = read_pgm(pgm)
    raw = b''.join(b'\x00' + pixels[y*w:(y+1)*w] for y in range(h))
    png = b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack(">IIBBBBB", w, h, 8, 0, 0, 0, 0)) + chunk(b'IDAT', zlib.compress(raw, 9)) + chunk(b'IEND', b'')
    pgm.with_suffix(".png").write_bytes(png)
)PY";
  out.close();
  // DASHBOARD_PREVIEW_PYTHON selects the interpreter; PGM files are kept either way.
  const char* python = std::getenv("DASHBOARD_PREVIEW_PYTHON");
#ifdef _WIN32
  const std::string interpreter = python != nullptr && python[0] != '\0' ? python : "python";
#else
  const std::string interpreter = python != nullptr && python[0] != '\0' ? python : "python3";
#endif
  const std::string cmd = interpreter + " \"" + script.string() + "\"";
  (void)std::system(cmd.c_str());
}

void writeMeasurements() {
  const char* dir = std::getenv("DASHBOARD_PREVIEW_DIR");
  if (dir == nullptr || dir[0] == '\0') {
    return;
  }
  RendererFixture fixture(480, 800);
  std::ofstream out(std::filesystem::path(dir) / "measurements.txt");
  const char* labels[] = {"Leitura",         "Restante",           "Progresso",    "Desde Sep 22",     "Previsão",
                          "Concluído",       "Média diária",       "Páginas/min",  "Sessões",          "Sessão média",
                          "Reading Time",    "Time Left",          "Since Sep 22", "Est. Finish",      "Finished",
                          "Daily Avg",       "Pages/Min",          "Avg Session",  "12 dias seguidos", "Leitor noturno",
                          "Leitor da tarde", "Ainda sem sequência"};
  out << "label,ui10,small\n";
  for (const char* label : labels) {
    out << label << ',' << fixture.renderer.getTextWidth(UI_10_FONT_ID, label) << ','
        << fixture.renderer.getTextWidth(SMALL_FONT_ID, label) << '\n';
  }
  const Rect homeRect{0, 50, 480, 690};
  const auto x4 = DashboardLayout::compute(fixture.renderer, homeRect, ptBrNormalContent(), false, 0);
  RendererFixture x3Fixture(528, 792);
  const auto x3 = DashboardLayout::compute(x3Fixture.renderer, homeRect, ptBrNormalContent(), true, 40);
  out << "X4 Pro stats width," << x4.statsRect.width << '\n';
  out << "X3 stats width," << x3.statsRect.width << '\n';
}

int lyraMenuTop(const GfxRenderer& renderer) {
  const int lh = renderer.getLineHeight(SMALL_FONT_ID);
  const int rowY = renderer.getScreenHeight() - 40 - 60 - 3 - lh - 4 + 31;
  return rowY - 3 - lh;
}

std::vector<std::string> lyraTitles() {
  return {"O Alienista",
          "Memórias Póstumas de Brás Cubas",
          "Dom Casmurro",
          "A Hora da Estrela",
          "Grande Sertão: Veredas em uma edição com título muito longo",
          "Capitães da Areia"};
}

void drawIconStripOutlines(const GfxRenderer& renderer, const int menuTop) {
  constexpr int slots = 7;
  const int rowY = menuTop + 3 + renderer.getLineHeight(SMALL_FONT_ID);
  const int slotW = renderer.getScreenWidth() / slots;
  for (int i = 0; i < slots; ++i) {
    renderer.drawRect(i * slotW + 2, menuTop, std::max(1, slotW - 4), rowY + 60 - menuTop);
  }
}

void renderLyraGridPreview(const char* name, RendererFixture& fixture, const LyraGridLayout::Grid& grid,
                           const Rect& rect, const int menuTop) {
  const char* dir = std::getenv("DASHBOARD_PREVIEW_DIR");
  if (dir == nullptr || dir[0] == '\0') {
    return;
  }
  constexpr float progress[] = {57.0f, 12.0f, -1.0f, 100.0f, 0.0f, 80.0f};
  const auto titles = lyraTitles();
  std::filesystem::create_directories(dir);
  fixture.renderer.clearScreen();
  fixture.renderer.drawRect(rect.x, rect.y, rect.width, menuTop - rect.y);
  for (int i = 0; i < LyraGridLayout::kTileCount; ++i) {
    const Rect tile = LyraGridLayout::tileRect(grid, i);
    const Rect cover = LyraGridLayout::coverRect(grid, tile);
    if (i == 0) {
      LyraGridLayout::drawSelection(fixture.renderer, cover);
    }
    fixture.renderer.fillRectDither(cover.x, cover.y, cover.width, cover.height, Color::LightGray);
    fixture.renderer.drawRect(cover.x, cover.y, cover.width, cover.height);
    if (i == 1 || i == 3) {
      LyraGridLayout::drawPinBadge(fixture.renderer, cover);
    }
    LyraGridLayout::drawProgressBar(fixture.renderer, cover, progress[i]);
    LyraGridLayout::drawTitle(fixture.renderer, tile, cover, titles[static_cast<size_t>(i)]);
  }
  drawIconStripOutlines(fixture.renderer, menuTop);
  writePgm(std::filesystem::path(dir) / (std::string(name) + ".pgm"), fixture.renderer);
}

void assertLyraGrid(const GfxRenderer& renderer, const LyraGridLayout::Grid& grid, const Rect& rect,
                    const int menuTop) {
  const auto titles = lyraTitles();
  std::vector<Rect> tiles;
  for (int i = 0; i < LyraGridLayout::kTileCount; ++i) {
    const Rect tile = LyraGridLayout::tileRect(grid, i);
    const Rect cover = LyraGridLayout::coverRect(grid, tile);
    tiles.push_back(tile);
    EXPECT_GE(tile.x, 0);
    EXPECT_GE(tile.y, rect.y);
    EXPECT_LE(tile.x + tile.width, renderer.getScreenWidth());
    EXPECT_LE(tile.y + tile.height, menuTop);
    expectInside(cover, renderer.getScreenWidth(), renderer.getScreenHeight());
    const auto lines = renderer.wrappedText(UI_10_FONT_ID, titles[static_cast<size_t>(i)].c_str(), tile.width, 2);
    for (const auto& line : lines) {
      EXPECT_LE(renderer.getTextWidth(UI_10_FONT_ID, line.c_str()), tile.width) << line;
    }
  }
  for (size_t i = 0; i < tiles.size(); ++i) {
    for (size_t j = i + 1; j < tiles.size(); ++j) {
      EXPECT_FALSE(intersects(tiles[i], tiles[j])) << i << " vs " << j;
    }
  }
}

TEST(DashboardLayoutTest, FitsSupportedScreensAndKeepsX4ProCover) {
  struct Case {
    int w;
    int h;
    bool touch;
    bool x3;
  };
  const Rect homeRect{0, 50, 480, 690};
  const auto ptBrNormal = ptBrNormalContent();
  const auto enNormal = enNormalContent();
  const auto noRtc = ptBrNoRtcContent();
  const auto worst = worstContent();
  for (const auto c : {Case{480, 800, true, false}, Case{480, 800, false, false}, Case{528, 792, false, true},
                       Case{800, 480, true, false}}) {
    for (const auto& content : {ptBrNormal, enNormal, noRtc, worst}) {
      SCOPED_TRACE(testing::Message() << c.w << "x" << c.h << " title=" << content.title);
      RendererFixture fixture(c.w, c.h);
      const int reserve = c.touch ? 0 : 40;
      const auto layout = DashboardLayout::compute(fixture.renderer, homeRect, content, c.x3, reserve);
      assertLayout(fixture.renderer, layout, c.touch, reserve);
      if (c.h > c.w && (content.title == ptBrNormal.title || content.title == enNormal.title) &&
          content.stats.size() == 7) {
        EXPECT_EQ(layout.statsLabelFontId, UI_10_FONT_ID);
        expectNoTruncatedLabels(layout);
      }
      if (c.w == 480 && c.h == 800 && c.touch && content.title == ptBrNormal.title && content.stats.size() == 7) {
        EXPECT_EQ(layout.statsLabelFontId, UI_10_FONT_ID);
        EXPECT_EQ(layout.coverRect.x, 20);
        EXPECT_EQ(layout.coverRect.y, 70);
        EXPECT_EQ(layout.coverRect.width, 296);
        EXPECT_EQ(layout.coverRect.height, 444);
        EXPECT_EQ(layout.statsRect.width, 129);
        EXPECT_EQ(layout.footerRect.y + layout.footerRect.height / 2, 743);
        writePreview("pt-br-normal", fixture, layout);
        writeBeforePreview(fixture);
      }
      if (c.w == 480 && c.h == 800 && c.touch && content.title == enNormal.title) {
        writePreview("en-normal", fixture, layout);
      }
      if (c.w == 480 && c.h == 800 && c.touch && content.title == noRtc.title && content.footerStats.size() == 2) {
        writePreview("pt-br-no-rtc", fixture, layout);
      }
      if (c.w == 480 && c.h == 800 && c.touch && content.title == worst.title) {
        writePreview("pt-br-worst", fixture, layout);
      }
      if (c.w == 528 && c.h == 792 && c.x3 && content.title == ptBrNormal.title && content.stats.size() == 7) {
        writePreview("x3-pt-br-normal", fixture, layout);
      }
      if (c.w == 800 && c.h == 480 && content.title == worst.title) {
        writePreview("landscape-worst", fixture, layout);
      }
    }
  }
  writeMeasurements();
  convertPreviewsToPng();
}

TEST(LyraGridLayoutTest, FitsAndRendersHomeGridPreview) {
  struct Case {
    int w;
    int h;
    const char* name;
  };
  for (const auto c : {Case{480, 800, "lyra-grid-x4pro"}, Case{528, 792, "lyra-grid-x3"}}) {
    RendererFixture fixture(c.w, c.h);
    const Rect rect{0, 28, fixture.renderer.getScreenWidth(), 650};
    const int menuTop = lyraMenuTop(fixture.renderer);
    const auto grid = LyraGridLayout::compute(fixture.renderer, rect, menuTop);
    assertLyraGrid(fixture.renderer, grid, rect, menuTop);
    if (c.w == 480 && c.h == 800) {
      EXPECT_EQ(grid.coverW, 136);
      EXPECT_EQ(grid.coverH, 204);
    }
    renderLyraGridPreview(c.name, fixture, grid, rect, menuTop);

    const char* dir = std::getenv("DASHBOARD_PREVIEW_DIR");
    if (dir != nullptr && dir[0] != '\0') {
      std::ofstream out(std::filesystem::path(dir) / (std::string(c.name) + "-metrics.txt"));
      const Rect lastRow = LyraGridLayout::tileRect(grid, 3);
      out << "startX," << grid.startX << '\n';
      out << "startY," << grid.startY << '\n';
      out << "tileW," << grid.tileW << '\n';
      out << "coverW," << grid.coverW << '\n';
      out << "coverH," << grid.coverH << '\n';
      out << "rowH," << grid.rowH << '\n';
      out << "rowGap," << grid.rowGap << '\n';
      out << "lastRowBottom," << lastRow.y + lastRow.height << '\n';
      out << "menuTop," << menuTop << '\n';
    }
  }
  convertPreviewsToPng();
}

struct InkBounds {
  int left = INT32_MAX;
  int top = INT32_MAX;
  int right = -1;
  int bottom = -1;
  int pixels = 0;
};

InkBounds inkBounds(const GfxRenderer& renderer, const int y0, const int y1) {
  InkBounds bounds;
  for (int y = std::max(0, y0); y < std::min(renderer.getScreenHeight(), y1); ++y) {
    for (int x = 0; x < renderer.getScreenWidth(); ++x) {
      if (!renderer.isPixelBlack(x, y)) continue;
      bounds.left = std::min(bounds.left, x);
      bounds.right = std::max(bounds.right, x);
      bounds.top = std::min(bounds.top, y);
      bounds.bottom = std::max(bounds.bottom, y);
      ++bounds.pixels;
    }
  }
  return bounds;
}

TEST(FluiDezBrandTest, BootAndSleepLockupIsCentredAndFits) {
  struct Case {
    int w;
    int h;
    const char* name;
  };
  for (const auto c : {Case{480, 800, "boot-x4pro"}, Case{528, 792, "boot-x3"}}) {
    SCOPED_TRACE(c.name);
    RendererFixture fixture(c.w, c.h);
    GfxRenderer& renderer = fixture.renderer;
    const int statusGap = 20;
    const int statusHeight = renderer.getLineHeight(SMALL_FONT_ID);
    renderer.clearScreen();
    const int top = FluiDezBrand::centredLockupTop(renderer, statusGap + statusHeight);
    const int bottom = FluiDezBrand::drawLockup(renderer, top);
    EXPECT_EQ(bottom - top, FluiDezBrand::lockupHeight());

    const int wordmarkTop = bottom - FluiDezLogo::kWordmarkHeight;
    const InkBounds symbol = inkBounds(renderer, top, top + FluiDezLogo::kSymbolSize);
    const InkBounds wordmark = inkBounds(renderer, wordmarkTop, bottom);
    ASSERT_GT(symbol.pixels, FluiDezLogo::kSymbolSize * FluiDezLogo::kSymbolSize / 3);
    ASSERT_GT(wordmark.pixels, 500);
    EXPECT_GE(symbol.top, top);
    EXPECT_LE(wordmark.bottom, bottom - 1);
    // The artwork is symmetric, so equal side margins prove the rotated packing
    // and drawIcon's portrait transform put it where the layout expects.
    EXPECT_NEAR(symbol.left, c.w - 1 - symbol.right, 2);
    EXPECT_NEAR(wordmark.left, c.w - 1 - wordmark.right, 3);
    EXPECT_GT(symbol.right - symbol.left, FluiDezLogo::kSymbolSize * 8 / 10);

    renderer.drawCenteredText(SMALL_FONT_ID, bottom + statusGap, "INICIANDO");
    renderer.drawCenteredText(SMALL_FONT_ID, c.h - 30, "1.6-fluidez7-x4-pro");
    EXPECT_LT(bottom + statusGap + statusHeight, c.h - 30);

    const char* dir = std::getenv("DASHBOARD_PREVIEW_DIR");
    if (dir != nullptr && dir[0] != '\0') {
      std::filesystem::create_directories(dir);
      writePgm(std::filesystem::path(dir) / (std::string(c.name) + ".pgm"), renderer);
      if (c.w == 480) {
        renderer.clearScreen();
        const int sleepBottom = FluiDezBrand::drawLockup(renderer, top);
        renderer.drawCenteredText(SMALL_FONT_ID, sleepBottom + statusGap, "EM REPOUSO");
        renderer.invertScreen();
        writePgm(std::filesystem::path(dir) / "sleep-x4pro.pgm", renderer);
      }
    }
  }
  convertPreviewsToPng();
}
}  // namespace
