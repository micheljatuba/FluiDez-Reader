#include "LyraGridLayout.h"

#include <GfxRenderer.h>

#include <algorithm>

#include "fontIds.h"

namespace LyraGridLayout {
namespace {
constexpr int kSidePadding = 12;
constexpr int kColumnGap = 12;
constexpr int kGridTopGap = 12;
constexpr int kGridMenuGap = 10;
constexpr int kMinRowGap = 10;
constexpr int kMaxRowGap = 28;
constexpr int kSelectionLineWidth = 3;
constexpr int kBarGap = 6;
constexpr int kBarHeight = 4;
constexpr int kTitleGap = 6;
constexpr int kTitleMaxLines = 2;
constexpr int kTitleFontId = UI_10_FONT_ID;
constexpr int kPinBadgeWidth = 14;
constexpr int kPinBadgeHeight = 20;
constexpr int kPinBadgeNotch = 5;
constexpr int kPinBadgeInset = 6;

int titleTop(const Rect& cover) { return cover.y + cover.height + kSelectionOutset + kBarGap + kBarHeight + kTitleGap; }
}  // namespace

Grid compute(const GfxRenderer& renderer, const Rect& rect, const int menuTop) {
  Grid grid;
  const int screenW = renderer.getScreenWidth();
  const int top = rect.y + kGridTopGap;
  const int bottom = std::min(rect.y + rect.height, menuTop - kGridMenuGap);
  const int available = std::max(0, bottom - top);

  grid.tileW = std::max(1, (screenW - 2 * kSidePadding - (kColumns - 1) * kColumnGap) / kColumns);
  grid.coverW = std::clamp(grid.tileW - 2 * kSelectionOutset, 1, kMaxCoverWidth);
  grid.coverH = (grid.coverW * 3) / 2;

  const int textH = kTitleMaxLines * renderer.getLineHeight(kTitleFontId);
  const int fixedRowH = 2 * kSelectionOutset + kBarGap + kBarHeight + kTitleGap + textH;
  const int minGapsH = (kRows - 1) * kMinRowGap;
  const int maxCoverH = (available - minGapsH) / kRows - fixedRowH;
  if (grid.coverH > maxCoverH) {
    grid.coverH = std::max(1, maxCoverH);
    grid.coverW = std::max(1, (grid.coverH * 2) / 3);
  }
  grid.rowH = fixedRowH + grid.coverH;

  const int spare = std::max(0, available - kRows * grid.rowH - minGapsH);
  grid.rowGap = std::min(kMaxRowGap, kMinRowGap + spare / kRows);
  const int usedH = kRows * grid.rowH + (kRows - 1) * grid.rowGap;
  grid.startY = top + std::max(0, (available - usedH) / 2);
  const int gridW = kColumns * grid.tileW + (kColumns - 1) * kColumnGap;
  grid.startX = std::max(0, (screenW - gridW) / 2);
  return grid;
}

Rect tileRect(const Grid& grid, const int index) {
  const int column = index % kColumns;
  const int row = index / kColumns;
  return Rect{grid.startX + column * (grid.tileW + kColumnGap), grid.startY + row * (grid.rowH + grid.rowGap),
              grid.tileW, grid.rowH};
}

Rect coverRect(const Grid& grid, const Rect& tile) {
  return Rect{tile.x + (tile.width - grid.coverW) / 2, tile.y + kSelectionOutset, grid.coverW, grid.coverH};
}

// A bookmark ribbon with a white halo so it stays visible on dark covers.
void drawPinBadge(const GfxRenderer& renderer, const Rect& cover) {
  const int x = cover.x + cover.width - kPinBadgeInset - kPinBadgeWidth;
  const int y = cover.y;
  renderer.fillRect(x - 1, y, kPinBadgeWidth + 2, kPinBadgeHeight + 1, false);
  const int middle = kPinBadgeWidth / 2;
  for (int row = 0; row < kPinBadgeHeight; ++row) {
    const int notchInset = row - (kPinBadgeHeight - kPinBadgeNotch) + 1;
    if (notchInset <= 0) {
      renderer.drawLine(x, y + row, x + kPinBadgeWidth - 1, y + row, true);
      continue;
    }
    renderer.drawLine(x, y + row, x + middle - notchInset, y + row, true);
    renderer.drawLine(x + middle + notchInset - 1, y + row, x + kPinBadgeWidth - 1, y + row, true);
  }
}

void drawProgressBar(const GfxRenderer& renderer, const Rect& cover, const float progressPercent) {
  if (progressPercent < 0.0f) return;
  const int y = cover.y + cover.height + kSelectionOutset + kBarGap;
  const float clamped = std::clamp(progressPercent, 0.0f, 100.0f);
  const int filledW = std::clamp(static_cast<int>((clamped / 100.0f) * cover.width + 0.5f), 0, cover.width);
  renderer.fillRectDither(cover.x, y, cover.width, kBarHeight, Color::LightGray);
  if (filledW > 0) {
    renderer.fillRect(cover.x, y, filledW, kBarHeight, true);
  }
}

void drawTitle(const GfxRenderer& renderer, const Rect& tile, const Rect& cover, const std::string& title) {
  const auto lines =
      renderer.wrappedText(kTitleFontId, title.c_str(), tile.width, kTitleMaxLines, EpdFontFamily::REGULAR);
  const int lineH = renderer.getLineHeight(kTitleFontId);
  int y = titleTop(cover);
  for (const auto& line : lines) {
    const int lineW = renderer.getTextWidth(kTitleFontId, line.c_str(), EpdFontFamily::REGULAR);
    renderer.drawText(kTitleFontId, tile.x + (tile.width - lineW) / 2, y, line.c_str(), true, EpdFontFamily::REGULAR);
    y += lineH;
  }
}

void drawSelection(const GfxRenderer& renderer, const Rect& cover) {
  renderer.drawRoundedRect(cover.x - kSelectionOutset, cover.y - kSelectionOutset, cover.width + 2 * kSelectionOutset,
                           cover.height + 2 * kSelectionOutset, kSelectionLineWidth, kCornerRadius + kSelectionOutset,
                           true);
}

}  // namespace LyraGridLayout
