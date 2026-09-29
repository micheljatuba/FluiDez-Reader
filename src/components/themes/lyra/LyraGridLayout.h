#pragma once

#include <string>

#include "components/themes/BaseTheme.h"

class GfxRenderer;

// Renderer-only geometry and text for the Lyra Grid home tiles, kept free of storage and HAL
// dependencies so host tests can render it with the firmware fonts.
namespace LyraGridLayout {

constexpr int kColumns = 3;
constexpr int kRows = 2;
constexpr int kTileCount = kColumns * kRows;
constexpr int kMaxCoverWidth = 136;
constexpr int kSelectionOutset = 4;
constexpr int kCornerRadius = 6;

struct Grid {
  int startX = 0;
  int startY = 0;
  int tileW = 0;
  int coverW = 0;
  int coverH = 0;
  int rowH = 0;
  int rowGap = 0;
};

// `rect` is the Home cover tile area; `menuTop` is the top of the bottom icon menu.
Grid compute(const GfxRenderer& renderer, const Rect& rect, int menuTop);
Rect tileRect(const Grid& grid, int index);
Rect coverRect(const Grid& grid, const Rect& tile);

void drawPinBadge(const GfxRenderer& renderer, const Rect& cover);
void drawProgressBar(const GfxRenderer& renderer, const Rect& cover, float progressPercent);
void drawTitle(const GfxRenderer& renderer, const Rect& tile, const Rect& cover, const std::string& title);
void drawSelection(const GfxRenderer& renderer, const Rect& cover);

}  // namespace LyraGridLayout
