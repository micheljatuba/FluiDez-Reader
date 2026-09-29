#include "DashboardLayout.h"

#include <GfxRenderer.h>

#include <algorithm>
#include <cctype>
#include <numeric>

#include "fontIds.h"

namespace DashboardLayout {
namespace {
constexpr int kContentInsetX4 = 20;
constexpr int kContentInsetX3 = 75;
constexpr int kTopInset = 20;
constexpr int kCoverStatsGap = 15;
constexpr int kPairInwardShiftX3 = 15;
constexpr int kTitleTopGap = 28;
constexpr int kTitleChapterGap = 8;
constexpr int kTextFooterGap = 10;
constexpr int kBookTitleMaxLines = 2;
constexpr int kBookChapterMaxLines = 2;
constexpr int kFooterIconSize = 24;
constexpr int kFooterIconTextGap = 18;
constexpr int kFooterBottomGap = 57;
constexpr int kStatsValueLabelGap = 1;

int right(const Rect& r) { return r.x + r.width; }
int bottom(const Rect& r) { return r.y + r.height; }
int contentInset(const int screenW) { return screenW >= 560 ? kContentInsetX3 : kContentInsetX4; }

int safeRight(const int screenW, const bool isX3) {
  return screenW - contentInset(screenW) - (isX3 ? kPairInwardShiftX3 : 0);
}

int coverX(const int screenW, const bool isX3) { return contentInset(screenW) + (isX3 ? kPairInwardShiftX3 : 0); }

Rect makeCoverRect(const GfxRenderer& renderer, const Rect& rect, const bool isX3, const int coverW) {
  const int w = std::clamp(coverW, 1, kMaxCoverWidth);
  return Rect{coverX(renderer.getScreenWidth(), isX3), rect.y + kTopInset, w, std::max(1, (w * 3) / 2)};
}

Rect footerIconRect(const int screenH, const int reserve, const int footerH) {
  const int bottomLimit = std::max(0, screenH - reserve);
  return Rect{0, std::max(0, bottomLimit - kFooterBottomGap - footerH / 2), 0, footerH};
}

int textWidthFor(const GfxRenderer& renderer, const bool isX3) {
  return std::max(1, safeRight(renderer.getScreenWidth(), isX3) - coverX(renderer.getScreenWidth(), isX3));
}

std::vector<std::string> wrapLines(const GfxRenderer& renderer, const int fontId, const std::string& text,
                                   const int width, const int maxLines, const bool bold) {
  if (text.empty() || maxLines <= 0) {
    return {};
  }
  return renderer.wrappedText(fontId, text.c_str(), std::max(1, width), maxLines,
                              bold ? EpdFontFamily::BOLD : EpdFontFamily::REGULAR);
}

int textBlockHeight(const GfxRenderer& renderer, const int titleLineCount, const int subtitleLineCount) {
  const int lineH = renderer.getLineHeight(UI_12_FONT_ID);
  int h = titleLineCount * lineH;
  if (subtitleLineCount > 0) {
    h += kTitleChapterGap + subtitleLineCount * lineH;
  }
  return h;
}

TextLine makeLine(const Rect& area, const int fontId, const bool bold, std::string text) {
  return TextLine{area, fontId, bold, std::move(text)};
}

void placeBookText(const GfxRenderer& renderer, Result& result, const Content& content, const int textW,
                   const int subtitleMaxLines) {
  const int lineH = renderer.getLineHeight(UI_12_FONT_ID);
  auto title = wrapLines(renderer, UI_12_FONT_ID, content.title, textW, kBookTitleMaxLines, true);
  auto subtitle = wrapLines(renderer, UI_12_FONT_ID, content.subtitle, textW, subtitleMaxLines, false);
  int y = bottom(result.coverRect) + kTitleTopGap;
  const int x = result.coverRect.x;
  result.titleLines.reserve(title.size());
  for (auto& line : title) {
    result.titleLines.push_back(makeLine(Rect{x, y, textW, lineH}, UI_12_FONT_ID, true, std::move(line)));
    y += lineH;
  }
  if (!subtitle.empty()) {
    y += kTitleChapterGap;
    result.subtitleLines.reserve(subtitle.size());
    for (auto& line : subtitle) {
      result.subtitleLines.push_back(makeLine(Rect{x, y, textW, lineH}, UI_12_FONT_ID, false, std::move(line)));
      y += lineH;
    }
  }
  result.textRect = Rect{x, bottom(result.coverRect) + kTitleTopGap, textW,
                         std::max(0, y - (bottom(result.coverRect) + kTitleTopGap))};
}

struct PreparedStatRow {
  std::string value;
  std::vector<std::string> labels;
  bool labelTruncated = false;
};

bool endsWithEllipsis(const std::string& text) {
  constexpr const char* ellipsis = "\xE2\x80\xA6";
  return text.size() >= 3 && text.compare(text.size() - 3, 3, ellipsis) == 0;
}

std::string normalizedWords(const std::string& text) {
  std::string out;
  bool inSpace = true;
  for (const unsigned char ch : text) {
    if (std::isspace(ch) != 0) {
      inSpace = true;
    } else {
      if (!out.empty() && inSpace) {
        out.push_back(' ');
      }
      out.push_back(static_cast<char>(ch));
      inSpace = false;
    }
  }
  return out;
}

bool labelWasTruncated(const std::string& original, const std::vector<std::string>& lines) {
  std::string joined;
  for (const auto& line : lines) {
    if (endsWithEllipsis(line)) {
      return true;
    }
    if (!joined.empty()) {
      joined.push_back(' ');
    }
    joined += line;
  }
  return normalizedWords(joined) != normalizedWords(original);
}

std::vector<PreparedStatRow> prepareStats(const GfxRenderer& renderer, const std::vector<StatRow>& rows,
                                          const int labelFontId, const int width, const int maxLabelLines) {
  std::vector<PreparedStatRow> prepared;
  prepared.reserve(rows.size());
  for (const auto& row : rows) {
    PreparedStatRow out;
    out.value = renderer.truncatedText(UI_12_FONT_ID, row.value.c_str(), width, EpdFontFamily::BOLD);
    out.labels = renderer.wrappedText(labelFontId, row.label.c_str(), std::max(1, width), maxLabelLines);
    out.labelTruncated = labelWasTruncated(row.label, out.labels);
    prepared.push_back(std::move(out));
  }
  return prepared;
}

bool anyLabelTruncated(const std::vector<PreparedStatRow>& rows) {
  return std::any_of(rows.begin(), rows.end(), [](const PreparedStatRow& row) { return row.labelTruncated; });
}

int statRowHeight(const GfxRenderer& renderer, const PreparedStatRow& row, const int labelFontId) {
  const int valueLineH = renderer.getLineHeight(UI_12_FONT_ID);
  const int labelLineH = renderer.getLineHeight(labelFontId);
  const int labelLines = std::max(1, static_cast<int>(row.labels.size()));
  return valueLineH + kStatsValueLabelGap + labelLineH * labelLines;
}

int statRowsHeight(const GfxRenderer& renderer, const std::vector<PreparedStatRow>& rows, const int labelFontId) {
  return std::accumulate(rows.begin(), rows.end(), 0, [&](const int total, const PreparedStatRow& row) {
    return total + statRowHeight(renderer, row, labelFontId);
  });
}

void placeStatsWithFont(const GfxRenderer& renderer, Result& result, const Content& content, const int labelFontId) {
  if (result.statsRect.width <= 0 || content.stats.empty()) {
    return;
  }
  auto prepared = prepareStats(renderer, content.stats, labelFontId, result.statsRect.width, 2);
  int rowsH = statRowsHeight(renderer, prepared, labelFontId);
  if (labelFontId == UI_10_FONT_ID && (anyLabelTruncated(prepared) || rowsH > result.coverRect.height)) {
    prepared = prepareStats(renderer, content.stats, SMALL_FONT_ID, result.statsRect.width, 2);
    rowsH = statRowsHeight(renderer, prepared, SMALL_FONT_ID);
    if (anyLabelTruncated(prepared) || rowsH > result.coverRect.height) {
      prepared = prepareStats(renderer, content.stats, SMALL_FONT_ID, result.statsRect.width, 1);
      rowsH = statRowsHeight(renderer, prepared, SMALL_FONT_ID);
    }
    result.statsLabelFontId = SMALL_FONT_ID;
  } else {
    result.statsLabelFontId = labelFontId;
  }
  while (!prepared.empty() && rowsH > result.coverRect.height) {
    prepared.pop_back();
    rowsH = statRowsHeight(renderer, prepared, result.statsLabelFontId);
  }

  const int valueLineH = renderer.getLineHeight(UI_12_FONT_ID);
  const int labelLineH = renderer.getLineHeight(result.statsLabelFontId);
  const int rightX = right(result.statsRect);
  const int gapCount = static_cast<int>(prepared.size()) - 1;
  const int remainingH = std::max(0, result.coverRect.height - rowsH);
  const int gap = gapCount > 0 ? remainingH / gapCount : 0;
  const int remainder = gapCount > 0 ? remainingH % gapCount : 0;
  int y = result.coverRect.y;
  result.stats.reserve(prepared.size());
  for (size_t i = 0; i < prepared.size(); ++i) {
    StatLine line;
    const int valueW = renderer.getTextWidth(UI_12_FONT_ID, prepared[i].value.c_str(), EpdFontFamily::BOLD);
    line.value =
        makeLine(Rect{rightX - valueW, y, valueW, valueLineH}, UI_12_FONT_ID, true, std::move(prepared[i].value));
    int labelY = y + valueLineH + kStatsValueLabelGap;
    line.labels.reserve(prepared[i].labels.size());
    for (auto& label : prepared[i].labels) {
      const int labelW = renderer.getTextWidth(result.statsLabelFontId, label.c_str());
      line.labels.push_back(makeLine(Rect{rightX - labelW, labelY, labelW, labelLineH}, result.statsLabelFontId, false,
                                     std::move(label)));
      labelY += labelLineH;
    }
    result.stats.push_back(std::move(line));
    y +=
        statRowHeight(renderer, prepared[i], result.statsLabelFontId) + gap + (static_cast<int>(i) < remainder ? 1 : 0);
  }
}

void placeFooter(const GfxRenderer& renderer, Result& result, const Content& content, const bool isX3,
                 const int buttonHintReserve) {
  const int footerH = content.footerStats.empty() ? kFooterIconSize
                                                  : renderer.getLineHeight(UI_12_FONT_ID) + kStatsValueLabelGap +
                                                        renderer.getLineHeight(UI_10_FONT_ID);
  const int footerTop = footerIconRect(renderer.getScreenHeight(), buttonHintReserve, footerH).y;
  const int centerY = footerTop + footerH / 2;
  const int screenW = renderer.getScreenWidth();
  const int inset = contentInset(screenW);
  result.footerRect = Rect{0, footerTop, screenW, footerH};
  if (!content.footerStats.empty()) {
    const int halfW = screenW / 2;
    const int maxTextW = std::max(1, halfW - inset * 2);
    const int valueLineH = renderer.getLineHeight(UI_12_FONT_ID);
    const int labelLineH = renderer.getLineHeight(UI_10_FONT_ID);
    const int topY = centerY - footerH / 2;
    const size_t statCount = std::min<size_t>(2, content.footerStats.size());
    result.footerStats.reserve(statCount);
    for (size_t i = 0; i < statCount; ++i) {
      const bool left = i == 0;
      const int anchorX = left ? result.coverRect.x : safeRight(screenW, isX3);
      const auto& item = content.footerStats[i];
      std::string label = renderer.truncatedText(UI_10_FONT_ID, item.label.c_str(), maxTextW);
      std::string value = renderer.truncatedText(UI_12_FONT_ID, item.value.c_str(), maxTextW, EpdFontFamily::BOLD);
      const int labelW = renderer.getTextWidth(UI_10_FONT_ID, label.c_str());
      const int valueW = renderer.getTextWidth(UI_12_FONT_ID, value.c_str(), EpdFontFamily::BOLD);
      const int labelX = left ? anchorX : anchorX - labelW;
      FooterStatLayout out;
      out.value = makeLine(Rect{labelX + (labelW - valueW) / 2, topY, valueW, valueLineH}, UI_12_FONT_ID, true,
                           std::move(value));
      out.label = makeLine(Rect{labelX, topY + valueLineH + kStatsValueLabelGap, labelW, labelLineH}, UI_10_FONT_ID,
                           false, std::move(label));
      result.footerStats.push_back(std::move(out));
    }
    return;
  }

  if (content.footerIcons.empty()) {
    return;
  }

  const int leftX = result.coverRect.x;
  const int rightLimit = safeRight(screenW, isX3);
  const int availableW = std::max(1, rightLimit - leftX);
  constexpr int minItemGap = 16;
  const int lineH = renderer.getLineHeight(UI_10_FONT_ID);
  int textMaxW[2] = {0, 0};
  const size_t iconCount = std::min<size_t>(2, content.footerIcons.size());
  for (size_t i = 0; i < iconCount; ++i) {
    textMaxW[i] = renderer.getTextWidth(UI_10_FONT_ID, content.footerIcons[i].label.c_str());
  }
  int overflow = static_cast<int>(iconCount) * (kFooterIconSize + kFooterIconTextGap) +
                 (iconCount > 1 ? minItemGap : 0) + textMaxW[0] + textMaxW[1] - availableW;
  while (overflow > 0 && (textMaxW[0] > 1 || textMaxW[1] > 1)) {
    const int idx = textMaxW[0] >= textMaxW[1] ? 0 : 1;
    const int shrink = std::min(overflow, std::max(0, textMaxW[idx] - 1));
    if (shrink > 0) {
      textMaxW[idx] -= shrink;
      overflow -= shrink;
    } else {
      const int other = 1 - idx;
      const int otherShrink = std::min(overflow, std::max(0, textMaxW[other] - 1));
      textMaxW[other] -= otherShrink;
      overflow -= otherShrink;
    }
  }
  result.footerIcons.reserve(iconCount);
  for (size_t i = 0; i < iconCount; ++i) {
    const bool left = i == 0;
    std::string label = renderer.truncatedText(UI_10_FONT_ID, content.footerIcons[i].label.c_str(), textMaxW[i]);
    const int labelW = renderer.getTextWidth(UI_10_FONT_ID, label.c_str());
    const int iconX = left ? leftX : rightLimit - labelW - kFooterIconTextGap - kFooterIconSize;
    const int labelX = iconX + kFooterIconSize + kFooterIconTextGap;
    FooterIconLayout out;
    out.icon = content.footerIcons[i].icon;
    out.iconRect = Rect{iconX, centerY - kFooterIconSize / 2, kFooterIconSize, kFooterIconSize};
    out.label = makeLine(Rect{labelX, centerY - lineH / 2, labelW, lineH}, UI_10_FONT_ID, false, std::move(label));
    result.footerIcons.push_back(std::move(out));
  }
}

int horizontalCoverWidth(const GfxRenderer& renderer, const bool isX3) {
  const int x = coverX(renderer.getScreenWidth(), isX3);
  const int maxW = safeRight(renderer.getScreenWidth(), isX3) - x - kCoverStatsGap - 1;
  return std::clamp(std::min(kMaxCoverWidth, maxW), 1, kMaxCoverWidth);
}
}  // namespace

Result compute(const GfxRenderer& renderer, const Rect& rect, const Content& content, const bool isX3,
               const int buttonHintReserve) {
  const int reserve = std::max(0, buttonHintReserve);
  const int textW = textWidthFor(renderer, isX3);
  const int footerH = content.footerStats.empty() ? kFooterIconSize
                                                  : renderer.getLineHeight(UI_12_FONT_ID) + kStatsValueLabelGap +
                                                        renderer.getLineHeight(UI_10_FONT_ID);
  const int footerTop = footerIconRect(renderer.getScreenHeight(), reserve, footerH).y;
  const int titleLines =
      static_cast<int>(wrapLines(renderer, UI_12_FONT_ID, content.title, textW, kBookTitleMaxLines, true).size());
  int subtitleLines =
      static_cast<int>(wrapLines(renderer, UI_12_FONT_ID, content.subtitle, textW, kBookChapterMaxLines, false).size());
  int coverW = horizontalCoverWidth(renderer, isX3);
  Result result;
  while (true) {
    result = Result{};
    result.coverRect = makeCoverRect(renderer, rect, isX3, coverW);
    const int availableTextH = footerTop - kTextFooterGap - (bottom(result.coverRect) + kTitleTopGap);
    while (subtitleLines > 0 && textBlockHeight(renderer, titleLines, subtitleLines) > availableTextH) {
      --subtitleLines;
    }
    if (textBlockHeight(renderer, titleLines, subtitleLines) <= availableTextH || coverW <= 1) {
      break;
    }
    const int titleH = textBlockHeight(renderer, titleLines, 0);
    const int maxCoverH = std::max(1, footerTop - kTextFooterGap - kTitleTopGap - titleH - result.coverRect.y);
    coverW = std::clamp(std::min(coverW - 1, (maxCoverH * 2) / 3), 1, coverW - 1);
  }

  result.statsRect =
      Rect{right(result.coverRect) + kCoverStatsGap, result.coverRect.y,
           std::max(0, safeRight(renderer.getScreenWidth(), isX3) - right(result.coverRect) - kCoverStatsGap),
           result.coverRect.height};
  placeBookText(renderer, result, content, textW, subtitleLines);
  placeFooter(renderer, result, content, isX3, reserve);
  placeStatsWithFont(renderer, result, content, UI_10_FONT_ID);
  return result;
}

void draw(const GfxRenderer& renderer, const Result& layout, const bool black, const bool invertedIcons) {
  for (const auto& row : layout.stats) {
    renderer.drawText(row.value.fontId, row.value.rect.x, row.value.rect.y, row.value.text.c_str(), black,
                      row.value.bold ? EpdFontFamily::BOLD : EpdFontFamily::REGULAR);
    for (const auto& label : row.labels) {
      renderer.drawText(label.fontId, label.rect.x, label.rect.y, label.text.c_str(), black);
    }
  }
  for (const auto& line : layout.titleLines) {
    renderer.drawText(line.fontId, line.rect.x, line.rect.y, line.text.c_str(), black, EpdFontFamily::BOLD);
  }
  for (const auto& line : layout.subtitleLines) {
    renderer.drawText(line.fontId, line.rect.x, line.rect.y, line.text.c_str(), black);
  }
  for (const auto& item : layout.footerIcons) {
    if (item.icon != nullptr) {
      if (invertedIcons) {
        renderer.drawIconInverted(item.icon, item.iconRect.x, item.iconRect.y, item.iconRect.width,
                                  item.iconRect.height);
      } else {
        renderer.drawIcon(item.icon, item.iconRect.x, item.iconRect.y, item.iconRect.width, item.iconRect.height);
      }
    }
    renderer.drawText(item.label.fontId, item.label.rect.x, item.label.rect.y, item.label.text.c_str(), black);
  }
  for (const auto& item : layout.footerStats) {
    renderer.drawText(item.value.fontId, item.value.rect.x, item.value.rect.y, item.value.text.c_str(), black,
                      EpdFontFamily::BOLD);
    renderer.drawText(item.label.fontId, item.label.rect.x, item.label.rect.y, item.label.text.c_str(), black);
  }
}

}  // namespace DashboardLayout
