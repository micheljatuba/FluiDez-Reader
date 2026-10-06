#include "FluiDezTheme.h"

#include <Bitmap.h>
#include <GfxRenderer.h>
#include <HalClock.h>
#include <HalStorage.h>
#include <I18n.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include "CrossPointSettings.h"
#include "RecentBooksStore.h"
#include "activities/reader/GlobalReadingStats.h"
#include "activities/reader/ReadingStatsUtils.h"
#include "components/TouchRegistry.h"
#include "components/UITheme.h"
#include "components/UiAppHelpers.h"
#include "fontIds.h"

namespace {
std::array<float, FluiDezTheme::kMaxHomeBooks> gHomeBookProgress = [] {
  std::array<float, FluiDezTheme::kMaxHomeBooks> values{};
  values.fill(-1.0f);
  return values;
}();
BookReadingStats gLeadBookStats;

constexpr int kSidePadding = 24;

// Upper-cases ASCII only: section labels are short UI strings and multi-byte
// UTF-8 letters must pass through untouched.
std::string upperAscii(const char* text) {
  std::string out = text != nullptr ? text : "";
  std::transform(out.begin(), out.end(), out.begin(),
                 [](const char c) { return (c >= 'a' && c <= 'z') ? static_cast<char>(c - 'a' + 'A') : c; });
  return out;
}

Rect fittedRect(const int srcW, const int srcH, const Rect& target) {
  if (srcW <= 0 || srcH <= 0 || target.width <= 0 || target.height <= 0) return target;
  const float scale = std::min(1.0f, std::min(static_cast<float>(target.width) / static_cast<float>(srcW),
                                              static_cast<float>(target.height) / static_cast<float>(srcH)));
  const int w = std::min(target.width, std::max(1, static_cast<int>(std::ceil(srcW * scale))));
  const int h = std::min(target.height, std::max(1, static_cast<int>(std::ceil(srcH * scale))));
  return Rect{target.x + (target.width - w) / 2, target.y + (target.height - h) / 2, w, h};
}

int percentValue(const float percent) { return static_cast<int>(std::clamp(percent, 0.0f, 100.0f) + 0.5f); }

// Stable per-title variation so a spine keeps its shape between visits.
uint32_t titleHash(const std::string& text) {
  uint32_t hash = 2166136261u;
  for (const char c : text) {
    hash ^= static_cast<uint8_t>(c);
    hash *= 16777619u;
  }
  return hash;
}

// Progress ring: black arc from 12 o'clock clockwise, thin outline for the rest.
void drawProgressRing(const GfxRenderer& renderer, const int cx, const int cy, const int radius, const int thickness,
                      const float percent) {
  const int inner = radius - thickness;
  const float sweep = std::clamp(percent, 0.0f, 100.0f) / 100.0f * 6.2831853f;
  const int outerSq = radius * radius;
  const int innerSq = inner * inner;
  if (sweep > 0.0f) {
    for (int dy = -radius; dy <= radius; ++dy) {
      for (int dx = -radius; dx <= radius; ++dx) {
        const int d2 = dx * dx + dy * dy;
        if (d2 > outerSq || d2 < innerSq) continue;
        float angle = std::atan2(static_cast<float>(dx), static_cast<float>(-dy));
        if (angle < 0.0f) angle += 6.2831853f;
        if (angle <= sweep) renderer.drawPixel(cx + dx, cy + dy, true);
      }
    }
  }
  for (const int r : {radius, inner}) {
    renderer.drawArc(r, cx, cy, -1, -1, 1, true);
    renderer.drawArc(r, cx, cy, 1, -1, 1, true);
    renderer.drawArc(r, cx, cy, 1, 1, 1, true);
    renderer.drawArc(r, cx, cy, -1, 1, 1, true);
  }
}

void drawCentered(const GfxRenderer& renderer, const int fontId, const int centerX, const int y, const char* text,
                  const bool black = true, const EpdFontFamily::Style style = EpdFontFamily::REGULAR) {
  const int w = renderer.getTextWidth(fontId, text, style);
  renderer.drawText(fontId, centerX - w / 2, y, text, black, style);
}
}  // namespace

// ---------------------------------------------------------------------------
// Shared FluiDez helpers
// ---------------------------------------------------------------------------

void FluiDezTheme::setHomeBookProgress(const int index, const float progressPercent) {
  if (index >= 0 && index < kMaxHomeBooks) gHomeBookProgress[index] = progressPercent;
}

void FluiDezTheme::setLeadBookStats(const BookReadingStats& stats) { gLeadBookStats = stats; }

void FluiDezTheme::clearHomeData() {
  gHomeBookProgress.fill(-1.0f);
  gLeadBookStats = BookReadingStats{};
}

float FluiDezTheme::homeBookProgress(const int index) {
  return (index >= 0 && index < kMaxHomeBooks) ? gHomeBookProgress[index] : -1.0f;
}

const BookReadingStats& FluiDezTheme::leadBookStats() { return gLeadBookStats; }

const char* FluiDezTheme::bookTitle(const RecentBook& book) {
  return book.title.empty() ? book.path.c_str() : book.title.c_str();
}

bool FluiDezTheme::drawCoverFitted(const GfxRenderer& renderer, const RecentBook& book, const Rect rect,
                                   const int thumbHeight, const int cornerRadius) {
  if (book.coverBmpPath.empty() || book.coverState == RecentBook::CoverState::Missing) return false;
  const std::string thumbPath = UITheme::getCoverThumbPath(book.coverBmpPath, thumbHeight);
  if (thumbPath.empty()) return false;

  HalFile file;
  if (!Storage.openFileForRead("HOME", thumbPath, file)) return false;
  bool drawn = false;
  Bitmap bitmap(file);
  if (bitmap.parseHeaders() == BmpReaderError::Ok) {
    const Rect target = fittedRect(bitmap.getWidth(), bitmap.getHeight(), rect);
    drawn = renderer.drawBitmap(bitmap, target.x, target.y, target.width, target.height);
    if (drawn && cornerRadius > 0) {
      renderer.maskRoundedRectOutsideCorners(target.x, target.y, target.width, target.height, cornerRadius);
    }
    if (drawn) renderer.drawRoundedRect(target.x, target.y, target.width, target.height, 1, cornerRadius, true);
  }
  file.close();
  return drawn;
}

void FluiDezTheme::drawCoverPlaceholder(const GfxRenderer& renderer, const RecentBook& book, const Rect rect,
                                        const int cornerRadius) {
  renderer.fillRoundedRect(rect.x, rect.y, rect.width, rect.height, cornerRadius, Color::DarkGray);
  // A dark "spine" edge keeps placeholders reading as books.
  renderer.fillRect(rect.x, rect.y, std::min(6, rect.width), rect.height, true);
  constexpr int pad = 10;
  const int textW = rect.width - pad * 2 - 6;
  if (textW <= 0) return;
  const auto lines = renderer.wrappedText(UI_10_FONT_ID, bookTitle(book), textW, 4, EpdFontFamily::BOLD);
  const int lineH = renderer.getLineHeight(UI_10_FONT_ID);
  int y = rect.y + rect.height - pad - static_cast<int>(lines.size()) * lineH;
  for (const auto& line : lines) {
    renderer.drawText(UI_10_FONT_ID, rect.x + 6 + pad, y, line.c_str(), false, EpdFontFamily::BOLD);
    y += lineH;
  }
}

void FluiDezTheme::drawSectionLabel(const GfxRenderer& renderer, const int x, const int y, const char* label) {
  renderer.drawText(SMALL_FONT_ID, x, y, upperAscii(label).c_str(), true);
}

void FluiDezTheme::drawProgressLine(const GfxRenderer& renderer, const int x, const int y, const int width,
                                    const int height, const float percent, const int radius) {
  if (width <= 0 || height <= 0) return;
  const int filled = std::clamp(static_cast<int>(width * std::clamp(percent, 0.0f, 100.0f) / 100.0f), 0, width);
  if (radius > 0) {
    renderer.fillRoundedRect(x, y, width, height, radius, Color::LightGray);
    if (filled > 0) renderer.fillRoundedRect(x, y, std::max(filled, radius * 2), height, radius, Color::Black);
  } else {
    renderer.fillRectDither(x, y, width, height, Color::LightGray);
    if (filled > 0) renderer.fillRect(x, y, filled, height, true);
  }
}

bool FluiDezTheme::estimateTimeLeft(const BookReadingStats& stats, const float progressPercent, uint32_t& seconds) {
  seconds = 0;
  if (stats.isCompleted) return false;
  if (stats.estimatedTimeLeftSeconds > 0) {
    seconds = stats.estimatedTimeLeftSeconds;
    return true;
  }
  if (progressPercent <= 0.0f || progressPercent >= 100.0f || stats.totalReadingSeconds < 120) return false;
  const float progress = progressPercent / 100.0f;
  const float estimate = static_cast<float>(stats.totalReadingSeconds) * (1.0f - progress) / progress;
  seconds = estimate > 0.0f ? static_cast<uint32_t>(estimate + 0.5f) : 0;
  return seconds > 0;
}

void FluiDezTheme::formatShortDuration(const uint32_t seconds, char* buf, const size_t len) {
  const uint32_t minutes = (seconds + 30u) / 60u;
  if (minutes < 60) {
    snprintf(buf, len, "%lu min", static_cast<unsigned long>(minutes));
    return;
  }
  const uint32_t hours = minutes / 60u;
  const uint32_t remainder = minutes % 60u;
  if (remainder == 0 || hours >= 10) {
    snprintf(buf, len, "%lu h", static_cast<unsigned long>(hours));
  } else {
    snprintf(buf, len, "%lu h %lu", static_cast<unsigned long>(hours), static_cast<unsigned long>(remainder));
  }
}

void FluiDezTheme::drawTextGridMenu(const GfxRenderer& renderer, const Rect rect, const int buttonCount,
                                    const int selectedIndex, const std::function<const char*(int index)>& buttonLabel,
                                    const bool pillSelection) const {
  if (buttonCount <= 0 || rect.height <= 0) return;
  constexpr int columns = 2;
  constexpr int columnGap = 12;
  const int rows = (buttonCount + columns - 1) / columns;
  const int rowH = std::min(60, rect.height / rows);
  const int cellW = (rect.width - kSidePadding * 2 - columnGap) / columns;
  const int lineH = renderer.getLineHeight(UI_12_FONT_ID);

  for (int i = 0; i < buttonCount; ++i) {
    const int col = i % columns;
    const int row = i / columns;
    const Rect cell{rect.x + kSidePadding + col * (cellW + columnGap), rect.y + row * rowH, cellW, rowH - 6};
    TouchRegistry::getInstance().add(Rect{cell.x, cell.y, cell.width, rowH}, i, TouchRegistry::Item);

    const bool selected = i == selectedIndex;
    const char* label = buttonLabel != nullptr ? buttonLabel(i) : "";
    if (label == nullptr) label = "";
    const auto text = renderer.truncatedText(UI_12_FONT_ID, label, cell.width - 24,
                                             selected ? EpdFontFamily::BOLD : EpdFontFamily::REGULAR);
    const int textY = cell.y + (cell.height - lineH) / 2;

    if (pillSelection) {
      if (selected) {
        renderer.fillRoundedRect(cell.x, cell.y, cell.width, cell.height, cell.height / 2, Color::Black);
      } else {
        renderer.drawRoundedRect(cell.x, cell.y, cell.width, cell.height, 1, cell.height / 2, true);
      }
      renderer.drawText(UI_12_FONT_ID, cell.x + 18, textY, text.c_str(), !selected,
                        selected ? EpdFontFamily::BOLD : EpdFontFamily::REGULAR);
    } else {
      renderer.fillRect(cell.x, cell.y, cell.width, selected ? 3 : 1, true);
      renderer.drawText(UI_12_FONT_ID, cell.x + 2, textY + 2, text.c_str(), true,
                        selected ? EpdFontFamily::BOLD : EpdFontFamily::REGULAR);
    }
  }
}

// ---------------------------------------------------------------------------
// Fluxo: typographic Home, no image reads
// ---------------------------------------------------------------------------

void FluiDezFluxoTheme::drawRecentBookCover(GfxRenderer& renderer, const Rect rect,
                                            const std::vector<RecentBook>& recentBooks, const int selectorIndex,
                                            bool& coverRendered, bool& /*coverBufferStored*/, bool& /*bufferRestored*/,
                                            const std::function<bool()>& /*storeCoverBuffer*/,
                                            const BookReadingStats* /*stats*/, float /*progressPercent*/,
                                            const GlobalReadingStats* /*globalStats*/,
                                            const char* /*currentChapterTitle*/) const {
  // Pure text: nothing to snapshot, so Home never allocates the cover buffer.
  coverRendered = false;
  if (recentBooks.empty()) {
    drawEmptyRecents(renderer, rect);
    return;
  }

  const int x = rect.x + kSidePadding + 4;
  const int w = rect.width - kSidePadding * 2 - 4;
  const RecentBook& lead = recentBooks[0];
  const float leadProgress = homeBookProgress(0);
  const int heroTop = rect.y + 14;
  int y = heroTop;

  drawSectionLabel(renderer, x, y, tr(STR_CONTINUE_READING));
  y += renderer.getLineHeight(SMALL_FONT_ID) + 8;

  const int titleLineH = renderer.getLineHeight(UI_12_FONT_ID);
  for (const auto& line : renderer.wrappedText(UI_12_FONT_ID, bookTitle(lead), w, 3, EpdFontFamily::BOLD)) {
    renderer.drawText(UI_12_FONT_ID, x, y, line.c_str(), true, EpdFontFamily::BOLD);
    y += titleLineH;
  }
  if (!lead.author.empty()) {
    y += 4;
    const auto author = renderer.truncatedText(UI_10_FONT_ID, lead.author.c_str(), w);
    renderer.drawText(UI_10_FONT_ID, x, y, author.c_str(), true);
    y += renderer.getLineHeight(UI_10_FONT_ID);
  }

  y += 16;
  char percentLabel[8];
  snprintf(percentLabel, sizeof(percentLabel), "%d%%", leadProgress >= 0.0f ? percentValue(leadProgress) : 0);
  const int percentW = renderer.getTextWidth(UI_10_FONT_ID, percentLabel, EpdFontFamily::BOLD);
  const int lineH10 = renderer.getLineHeight(UI_10_FONT_ID);
  drawProgressLine(renderer, x, y + lineH10 / 2 - 3, w - percentW - 12, 6, std::max(0.0f, leadProgress));
  renderer.drawText(UI_10_FONT_ID, x + w - percentW, y, percentLabel, true, EpdFontFamily::BOLD);
  y += lineH10 + 4;

  const BookReadingStats& stats = leadBookStats();
  char buf[24];
  char meta[48];
  uint32_t leftSeconds = 0;
  if (estimateTimeLeft(stats, leadProgress, leftSeconds)) {
    formatShortDuration(leftSeconds, buf, sizeof(buf));
    snprintf(meta, sizeof(meta), "~%s %s", buf, tr(STR_FLUIDEZ_LEFT));
    renderer.drawText(SMALL_FONT_ID, x, y, meta, true);
  }
  if (stats.totalReadingSeconds >= 60) {
    formatShortDuration(stats.totalReadingSeconds, buf, sizeof(buf));
    snprintf(meta, sizeof(meta), "%s %s", buf, tr(STR_FLUIDEZ_READ_TOTAL));
    const int metaW = renderer.getTextWidth(SMALL_FONT_ID, meta);
    renderer.drawText(SMALL_FONT_ID, x + w - metaW, y, meta, true);
  }
  y += renderer.getLineHeight(SMALL_FONT_ID);

  TouchRegistry::getInstance().add(Rect{rect.x, heroTop - 6, rect.width, y - heroTop + 12}, 0, TouchRegistry::Cover);
  if (selectorIndex == 0) {
    renderer.fillRect(x - 14, heroTop, 5, y - heroTop, true);
  }

  const int visible = std::min(static_cast<int>(recentBooks.size()), FluiDezMetrics::fluxo.homeRecentBooksCount);
  if (visible <= 1) return;

  y += 26;
  drawSectionLabel(renderer, x, y, tr(STR_FLUIDEZ_UP_NEXT));
  y += renderer.getLineHeight(SMALL_FONT_ID) + 6;

  const int rowH = std::max(40, std::min(46, (rect.y + rect.height - y) / (visible - 1)));
  const int rowTextH = renderer.getLineHeight(UI_12_FONT_ID);
  for (int i = 1; i < visible; ++i) {
    const Rect row{x - 8, y, w + 8, rowH};
    TouchRegistry::getInstance().add(row, i, TouchRegistry::Cover);
    const bool selected = selectorIndex == i;
    if (selected) {
      renderer.fillRoundedRect(row.x, row.y, row.width, row.height, 8, Color::Black);
    } else {
      renderer.fillRect(row.x, row.y, row.width, i == 1 ? 2 : 1, true);
    }

    const float progress = homeBookProgress(i);
    char right[12];
    if (progress > 0.0f) {
      snprintf(right, sizeof(right), "%d%%", percentValue(progress));
    } else {
      snprintf(right, sizeof(right), "%s", tr(STR_FLUIDEZ_NEW));
    }
    const int rightW = renderer.getTextWidth(SMALL_FONT_ID, right);
    const auto title = renderer.truncatedText(UI_12_FONT_ID, bookTitle(recentBooks[i]), row.width - rightW - 32);
    const int textY = row.y + (row.height - rowTextH) / 2;
    renderer.drawText(UI_12_FONT_ID, row.x + 8, textY, title.c_str(), !selected);
    renderer.drawText(SMALL_FONT_ID, row.x + row.width - rightW - 8,
                      row.y + (row.height - renderer.getLineHeight(SMALL_FONT_ID)) / 2, right, !selected);
    y += rowH;
  }
}

void FluiDezFluxoTheme::drawButtonMenu(GfxRenderer& renderer, const Rect rect, const int buttonCount,
                                       const int selectedIndex,
                                       const std::function<const char*(int index)>& buttonLabel,
                                       const std::function<UIIcon(int index)>& /*rowIcon*/) const {
  drawTextGridMenu(renderer, rect, buttonCount, selectedIndex, buttonLabel, false);
}

// ---------------------------------------------------------------------------
// Cartões: modular cards, one cached cover snapshot
// ---------------------------------------------------------------------------

void FluiDezCardsTheme::drawRecentBookCover(GfxRenderer& renderer, const Rect rect,
                                            const std::vector<RecentBook>& recentBooks, const int selectorIndex,
                                            bool& coverRendered, bool& coverBufferStored, bool& /*bufferRestored*/,
                                            const std::function<bool()>& storeCoverBuffer,
                                            const BookReadingStats* /*stats*/, float /*progressPercent*/,
                                            const GlobalReadingStats* globalStats,
                                            const char* /*currentChapterTitle*/) const {
  if (recentBooks.empty()) {
    drawEmptyRecents(renderer, rect);
    return;
  }

  constexpr int pad = 18;
  constexpr int cardPad = 14;
  constexpr int cardH = 214;
  constexpr int gap = 12;
  const int x = rect.x + pad;
  const int w = rect.width - pad * 2;
  const int thumbH = FluiDezMetrics::cards.homeCoverHeight;
  const Rect card{x, rect.y + 4, w, cardH};
  const Rect hero{card.x + cardPad, card.y + cardPad, 124, cardH - cardPad * 2};

  const bool showStats = globalStats != nullptr && SETTINGS.shouldTrackReadingStats();
  const int statsH = showStats ? 66 : 0;
  const int recentsLabelY = card.y + cardH + gap + (showStats ? statsH + gap : 0);
  const int recentsY = recentsLabelY + renderer.getLineHeight(SMALL_FONT_ID) + 6;
  const int recentCount =
      std::min(static_cast<int>(recentBooks.size()), FluiDezMetrics::cards.homeRecentBooksCount) - 1;
  constexpr int recentSlots = 3;
  const int slotW = (w - (recentSlots - 1) * gap) / recentSlots;
  const int recentH = 126;
  auto recentRect = [&](const int slot) { return Rect{x + slot * (slotW + gap), recentsY, slotW, recentH}; };

  // Static artwork goes into the snapshot; everything that follows the
  // selection or progress is repainted on top each frame.
  if (!coverRendered) {
    if (!drawCoverFitted(renderer, recentBooks[0], hero, thumbH, 8)) {
      drawCoverPlaceholder(renderer, recentBooks[0], hero, 8);
    }
    for (int slot = 0; slot < recentCount; ++slot) {
      const RecentBook& book = recentBooks[slot + 1];
      const Rect slotRect = recentRect(slot);
      const Rect coverSlot{slotRect.x + (slotRect.width - 84) / 2, slotRect.y, 84, slotRect.height};
      if (!drawCoverFitted(renderer, book, coverSlot, thumbH, 6)) drawCoverPlaceholder(renderer, book, coverSlot, 6);
    }
    coverBufferStored = storeCoverBuffer();
    coverRendered = coverBufferStored;
  }

  // Hero card
  TouchRegistry::getInstance().add(card, 0, TouchRegistry::Cover);
  const bool leadSelected = selectorIndex == 0;
  renderer.drawRoundedRect(card.x, card.y, card.width, card.height, leadSelected ? 4 : 2, 16, true);

  const RecentBook& lead = recentBooks[0];
  const float leadProgress = homeBookProgress(0);
  const int tx = hero.x + hero.width + 16;
  const int tw = card.x + card.width - cardPad - tx;
  int y = card.y + cardPad + 2;
  drawSectionLabel(renderer, tx, y, tr(STR_FLUIDEZ_READING_NOW));
  y += renderer.getLineHeight(SMALL_FONT_ID) + 6;
  const int titleH = renderer.getLineHeight(UI_12_FONT_ID);
  for (const auto& line : renderer.wrappedText(UI_12_FONT_ID, bookTitle(lead), tw, 3, EpdFontFamily::BOLD)) {
    renderer.drawText(UI_12_FONT_ID, tx, y, line.c_str(), true, EpdFontFamily::BOLD);
    y += titleH;
  }
  if (!lead.author.empty()) {
    y += 2;
    renderer.drawText(UI_10_FONT_ID, tx, y, renderer.truncatedText(UI_10_FONT_ID, lead.author.c_str(), tw).c_str(),
                      true);
  }

  const int lineH10 = renderer.getLineHeight(UI_10_FONT_ID);
  const int bottomY = card.y + card.height - cardPad - lineH10;
  char percentLabel[8];
  snprintf(percentLabel, sizeof(percentLabel), "%d%%", leadProgress >= 0.0f ? percentValue(leadProgress) : 0);
  renderer.drawText(UI_10_FONT_ID, tx, bottomY, percentLabel, true, EpdFontFamily::BOLD);
  uint32_t leftSeconds = 0;
  if (estimateTimeLeft(leadBookStats(), leadProgress, leftSeconds)) {
    char buf[24];
    formatShortDuration(leftSeconds, buf, sizeof(buf));
    const int bufW = renderer.getTextWidth(SMALL_FONT_ID, buf);
    renderer.drawText(SMALL_FONT_ID, tx + tw - bufW, bottomY + 2, buf, true);
  }
  drawProgressLine(renderer, tx, bottomY - 16, tw, 10, std::max(0.0f, leadProgress), 5);

  // Stats cards
  if (showStats) {
    const int statY = card.y + cardH + gap;
    const int statW = (w - 2 * gap) / 3;
    char values[3][16];
    formatShortDuration(globalStats->totalReadingSeconds, values[0], sizeof(values[0]));
    ReadingStatsDateTime today;
    const bool hasToday = halClock.isAvailable() && getCurrentLocalReadingStatsDateTime(today);
    snprintf(values[1], sizeof(values[1]), "%u",
             static_cast<unsigned>(globalStats->currentReadingStreak(hasToday ? &today.date : nullptr)));
    snprintf(values[2], sizeof(values[2]), "%lu", static_cast<unsigned long>(globalStats->completedBooks));
    const char* labels[3] = {tr(STR_FLUIDEZ_READ_TOTAL), tr(STR_FLUIDEZ_STREAK), tr(STR_FLUIDEZ_FINISHED)};
    for (int i = 0; i < 3; ++i) {
      const int sx = x + i * (statW + gap);
      renderer.drawRoundedRect(sx, statY, statW, statsH, 1, 14, true);
      renderer.drawText(UI_12_FONT_ID, sx + 12, statY + 10, values[i], true, EpdFontFamily::BOLD);
      const auto label = renderer.truncatedText(SMALL_FONT_ID, labels[i], statW - 20);
      renderer.drawText(SMALL_FONT_ID, sx + 12, statY + statsH - 10 - renderer.getLineHeight(SMALL_FONT_ID),
                        label.c_str(), true);
    }
  }

  // Recents row
  if (recentCount > 0) {
    drawSectionLabel(renderer, x + 4, recentsLabelY, tr(STR_RECENTS));
    for (int slot = 0; slot < recentCount; ++slot) {
      const Rect slotRect = recentRect(slot);
      TouchRegistry::getInstance().add(slotRect, slot + 1, TouchRegistry::Cover);
      if (selectorIndex == slot + 1) {
        const Rect frame{slotRect.x + (slotRect.width - 84) / 2 - 5, slotRect.y - 5, 94, slotRect.height + 10};
        renderer.drawRoundedRect(frame.x, frame.y, frame.width, frame.height, 3, 10, true);
      }
    }
    const int selectedBook = selectorIndex >= 1 && selectorIndex <= recentCount ? selectorIndex : -1;
    if (selectedBook > 0) {
      char caption[96];
      const float progress = homeBookProgress(selectedBook);
      if (progress > 0.0f) {
        snprintf(caption, sizeof(caption), "%s  ·  %d%%", bookTitle(recentBooks[selectedBook]), percentValue(progress));
      } else {
        snprintf(caption, sizeof(caption), "%s", bookTitle(recentBooks[selectedBook]));
      }
      const auto text = renderer.truncatedText(UI_10_FONT_ID, caption, w);
      renderer.drawText(UI_10_FONT_ID, x + 4, recentsY + recentH + 10, text.c_str(), true);
    }
  }
}

void FluiDezCardsTheme::drawButtonMenu(GfxRenderer& renderer, const Rect rect, const int buttonCount,
                                       const int selectedIndex,
                                       const std::function<const char*(int index)>& buttonLabel,
                                       const std::function<UIIcon(int index)>& rowIcon) const {
  if (buttonCount <= 0 || rect.height <= 0) return;
  constexpr int pad = 18;
  constexpr int gap = 10;
  constexpr int columns = 4;
  const int rows = (buttonCount + columns - 1) / columns;
  const int tileW = (rect.width - pad * 2 - (columns - 1) * gap) / columns;
  const int tileH = std::min(92, (rect.height - (rows - 1) * gap) / rows);
  const int smallH = renderer.getLineHeight(SMALL_FONT_ID);

  for (int i = 0; i < buttonCount; ++i) {
    const Rect tile{rect.x + pad + (i % columns) * (tileW + gap), rect.y + (i / columns) * (tileH + gap), tileW, tileH};
    TouchRegistry::getInstance().add(tile, i, TouchRegistry::Item);
    const bool selected = i == selectedIndex;
    if (selected) {
      renderer.fillRoundedRect(tile.x, tile.y, tile.width, tile.height, 14, Color::Black);
    } else {
      renderer.drawRoundedRect(tile.x, tile.y, tile.width, tile.height, 1, 14, true);
    }

    const freeink::Icon* icon = rowIcon != nullptr ? iconForName(rowIcon(i), 32) : nullptr;
    const char* label = buttonLabel != nullptr ? buttonLabel(i) : "";
    if (label == nullptr) label = "";
    const auto lines = renderer.wrappedText(SMALL_FONT_ID, label, tile.width - 10, icon != nullptr ? 2 : 3);
    const int iconH = icon != nullptr ? 32 + 4 : 0;
    const int blockH = iconH + static_cast<int>(lines.size()) * smallH;
    int y = tile.y + std::max(4, (tile.height - blockH) / 2);
    if (icon != nullptr) {
      drawLucideIcon(renderer, *icon, tile.x + (tile.width - 32) / 2, y, !selected);
      y += iconH;
    }
    for (const auto& line : lines) {
      drawCentered(renderer, SMALL_FONT_ID, tile.x + tile.width / 2, y, line.c_str(), !selected);
      y += smallH;
    }
  }
}

// ---------------------------------------------------------------------------
// Estante: one cover, progress ring and a shelf of text spines
// ---------------------------------------------------------------------------

void FluiDezShelfTheme::drawRecentBookCover(GfxRenderer& renderer, const Rect rect,
                                            const std::vector<RecentBook>& recentBooks, const int selectorIndex,
                                            bool& coverRendered, bool& coverBufferStored, bool& /*bufferRestored*/,
                                            const std::function<bool()>& storeCoverBuffer,
                                            const BookReadingStats* /*stats*/, float /*progressPercent*/,
                                            const GlobalReadingStats* /*globalStats*/,
                                            const char* /*currentChapterTitle*/) const {
  if (recentBooks.empty()) {
    drawEmptyRecents(renderer, rect);
    return;
  }

  const int x = rect.x + kSidePadding;
  const int w = rect.width - kSidePadding * 2;
  const int thumbH = FluiDezMetrics::shelf.homeCoverHeight;
  const RecentBook& lead = recentBooks[0];
  const float leadProgress = homeBookProgress(0);
  const Rect cover{x, rect.y + 10, thumbH * 2 / 3, thumbH};
  const int ringCx = cover.x + cover.width + (x + w - cover.x - cover.width) / 2;
  const int ringCy = cover.y + 74;
  constexpr int ringR = 64;

  if (!coverRendered) {
    if (!drawCoverFitted(renderer, lead, cover, thumbH, 6)) drawCoverPlaceholder(renderer, lead, cover, 6);
    drawProgressRing(renderer, ringCx, ringCy, ringR, 12, std::max(0.0f, leadProgress));
    coverBufferStored = storeCoverBuffer();
    coverRendered = coverBufferStored;
  }

  TouchRegistry::getInstance().add(Rect{cover.x, cover.y, w, cover.height}, 0, TouchRegistry::Cover);
  char percentLabel[8];
  snprintf(percentLabel, sizeof(percentLabel), "%d%%", leadProgress >= 0.0f ? percentValue(leadProgress) : 0);
  const int lineH12 = renderer.getLineHeight(UI_12_FONT_ID);
  drawCentered(renderer, UI_12_FONT_ID, ringCx, ringCy - lineH12 / 2, percentLabel, true, EpdFontFamily::BOLD);

  int infoY = ringCy + ringR + 10;
  uint32_t leftSeconds = 0;
  if (estimateTimeLeft(leadBookStats(), leadProgress, leftSeconds)) {
    char buf[24];
    char meta[48];
    formatShortDuration(leftSeconds, buf, sizeof(buf));
    snprintf(meta, sizeof(meta), "%s %s", buf, tr(STR_FLUIDEZ_LEFT));
    drawCentered(renderer, SMALL_FONT_ID, ringCx, infoY, meta);
  }
  infoY += renderer.getLineHeight(SMALL_FONT_ID) + 8;

  // "Continue" pill doubles as the lead book's selection state.
  const char* continueLabel = tr(STR_FLUIDEZ_CONTINUE);
  const int pillW = renderer.getTextWidth(UI_10_FONT_ID, continueLabel, EpdFontFamily::BOLD) + 36;
  const int pillH = renderer.getLineHeight(UI_10_FONT_ID) + 16;
  const int pillX = ringCx - pillW / 2;
  const bool leadSelected = selectorIndex == 0;
  if (leadSelected) {
    renderer.fillRoundedRect(pillX, infoY, pillW, pillH, pillH / 2, Color::Black);
  } else {
    renderer.drawRoundedRect(pillX, infoY, pillW, pillH, 2, pillH / 2, true);
  }
  drawCentered(renderer, UI_10_FONT_ID, ringCx, infoY + 8, continueLabel, !leadSelected, EpdFontFamily::BOLD);

  // Title block under the cover.
  int y = cover.y + cover.height + 12;
  const int titleH = renderer.getLineHeight(UI_12_FONT_ID);
  for (const auto& line : renderer.wrappedText(UI_12_FONT_ID, bookTitle(lead), w, 2, EpdFontFamily::BOLD)) {
    renderer.drawText(UI_12_FONT_ID, x, y, line.c_str(), true, EpdFontFamily::BOLD);
    y += titleH;
  }
  if (!lead.author.empty()) {
    const auto author = renderer.truncatedText(UI_10_FONT_ID, lead.author.c_str(), w);
    renderer.drawText(UI_10_FONT_ID, x, y + 2, author.c_str(), true);
    y += renderer.getLineHeight(UI_10_FONT_ID) + 2;
  }

  // Shelf of spines, sitting on a black board at the bottom of the tile.
  const int captionH = renderer.getLineHeight(UI_10_FONT_ID) + 8;
  const int boardY = rect.y + rect.height - captionH - 5;
  const int labelY = std::max(y + 12, boardY - 172);
  drawSectionLabel(renderer, x, labelY, tr(STR_FLUIDEZ_YOUR_SHELF));
  const int shelfTop = labelY + renderer.getLineHeight(SMALL_FONT_ID) + 6;
  const int maxSpineH = std::max(60, boardY - shelfTop - 12);
  renderer.fillRect(x, boardY, w, 5, true);

  const int visible = std::min(static_cast<int>(recentBooks.size()), FluiDezMetrics::shelf.homeRecentBooksCount);
  constexpr Color spineColors[4] = {Color::Black, Color::DarkGray, Color::LightGray, Color::White};
  const int smallH = renderer.getLineHeight(SMALL_FONT_ID);
  int sx = x + 6;
  for (int i = 0; i < visible; ++i) {
    const uint32_t hash = titleHash(recentBooks[i].title.empty() ? recentBooks[i].path : recentBooks[i].title);
    const int spineW = 36 + static_cast<int>(hash % 3) * 6;
    const int spineH = std::min(maxSpineH, maxSpineH - 40 + static_cast<int>((hash >> 4) % 5) * 10);
    if (sx + spineW > x + w) break;
    const bool selected = selectorIndex == i;
    const int lift = selected ? 10 : 0;
    const Rect spine{sx, boardY - spineH - lift, spineW, spineH};
    const Color color = spineColors[i % 4];
    TouchRegistry::getInstance().add(Rect{spine.x, boardY - maxSpineH - 10, spine.width, maxSpineH + 10}, i,
                                     TouchRegistry::Cover);

    if (color == Color::Black) {
      renderer.fillRoundedRect(spine.x, spine.y, spine.width, spine.height, 3, true, true, false, false, Color::Black);
    } else {
      renderer.fillRoundedRect(spine.x, spine.y, spine.width, spine.height, 3, true, true, false, false, color);
      renderer.drawRoundedRect(spine.x, spine.y, spine.width, spine.height, 1, 3, true, true, false, false, true);
    }
    if (selected) {
      renderer.drawRoundedRect(spine.x - 4, spine.y - 4, spine.width + 8, spine.height + 4, 2, 5, true, true, false,
                               false, true);
    }

    const bool lightText = color == Color::Black || color == Color::DarkGray;
    const auto title =
        renderer.truncatedText(SMALL_FONT_ID, bookTitle(recentBooks[i]), spine.height - 16, EpdFontFamily::BOLD);
    const int textW = renderer.getTextWidth(SMALL_FONT_ID, title.c_str(), EpdFontFamily::BOLD);
    renderer.drawTextRotated90CW(SMALL_FONT_ID, spine.x + (spine.width - smallH) / 2,
                                 spine.y + (spine.height + textW) / 2, title.c_str(), !lightText, EpdFontFamily::BOLD);
    sx += spineW + 6;
  }

  // Caption names the selected spine (or the lead book while the menu is focused).
  const int captionBook = selectorIndex >= 0 && selectorIndex < visible ? selectorIndex : 0;
  char caption[96];
  const float progress = homeBookProgress(captionBook);
  if (progress > 0.0f) {
    snprintf(caption, sizeof(caption), "%s  ·  %d%%", bookTitle(recentBooks[captionBook]), percentValue(progress));
  } else {
    snprintf(caption, sizeof(caption), "%s  ·  %s", bookTitle(recentBooks[captionBook]), tr(STR_FLUIDEZ_NEW));
  }
  const auto text = renderer.truncatedText(UI_10_FONT_ID, caption, w);
  renderer.drawText(UI_10_FONT_ID, x, boardY + 10, text.c_str(), true, EpdFontFamily::BOLD);
}

void FluiDezShelfTheme::drawButtonMenu(GfxRenderer& renderer, const Rect rect, const int buttonCount,
                                       const int selectedIndex,
                                       const std::function<const char*(int index)>& buttonLabel,
                                       const std::function<UIIcon(int index)>& /*rowIcon*/) const {
  drawTextGridMenu(renderer, rect, buttonCount, selectedIndex, buttonLabel, true);
}
