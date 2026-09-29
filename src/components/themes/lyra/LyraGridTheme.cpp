#include "LyraGridTheme.h"

#include <Bitmap.h>
#include <GfxRenderer.h>
#include <HalStorage.h>

#include <algorithm>
#include <array>
#include <string>
#include <vector>

#include "RecentBooksStore.h"
#include "components/TouchRegistry.h"
#include "components/UITheme.h"
#include "components/UiAppHelpers.h"
#include "components/themes/lyra/LyraGridLayout.h"

static_assert(LyraGridTheme::kTileCount == LyraGridMetrics::values.homeRecentBooksCount,
              "Lyra Grid shows one recent book per tile");

namespace {
constexpr int kMissingIconSize = 32;

std::array<float, LyraGridTheme::kTileCount> gTileProgress = [] {
  std::array<float, LyraGridTheme::kTileCount> progress{};
  progress.fill(-1.0f);
  return progress;
}();

bool drawCoverBitmap(const GfxRenderer& renderer, const RecentBook& book, const Rect& target) {
  if (book.coverBmpPath.empty()) return false;
  const std::string thumbPath = UITheme::getCoverThumbPath(book.coverBmpPath, LyraGridMetrics::values.homeCoverHeight);
  FsFile file;
  if (!Storage.openFileForRead("HOME", thumbPath, file)) return false;

  bool drawn = false;
  Bitmap bitmap(file);
  if (bitmap.parseHeaders() == BmpReaderError::Ok && bitmap.getWidth() > 0 && bitmap.getHeight() > 0) {
    const int srcW = bitmap.getWidth();
    const int srcH = bitmap.getHeight();
    // Wider covers are centre-cropped by whole pixels. drawBitmap skips floor(srcW * cropX / 2) columns per side,
    // so the extra 0.5 px keeps that count exact and the cropped width below the target, avoiding any rescale.
    const int cropPixX = srcW > target.width ? (srcW - target.width + 1) / 2 : 0;
    const float cropX = cropPixX > 0 ? (2.0f * static_cast<float>(cropPixX) + 0.5f) / static_cast<float>(srcW) : 0.0f;
    const int croppedW = srcW - 2 * cropPixX;
    const float scale = std::min(1.0f, std::min(static_cast<float>(target.width) / static_cast<float>(croppedW),
                                                static_cast<float>(target.height) / static_cast<float>(srcH)));
    const int drawnW = std::clamp(static_cast<int>(static_cast<float>(croppedW) * scale), 1, target.width);
    const int drawnH = std::clamp(static_cast<int>(static_cast<float>(srcH) * scale), 1, target.height);
    const int x = target.x + (target.width - drawnW) / 2;
    const int y = target.y + (target.height - drawnH) / 2;
    if (renderer.drawBitmap(bitmap, x, y, drawnW, drawnH, cropX)) {
      renderer.maskRoundedRectOutsideCorners(x, y, drawnW, drawnH, LyraGridLayout::kCornerRadius, Color::White);
      renderer.drawRoundedRect(x, y, drawnW, drawnH, 1, LyraGridLayout::kCornerRadius, true);
      drawn = true;
    }
  }
  file.close();
  return drawn;
}

void drawMissingCover(const GfxRenderer& renderer, const Rect& target) {
  renderer.fillRoundedRect(target.x, target.y, target.width, target.height, LyraGridLayout::kCornerRadius,
                           Color::White);
  renderer.drawRoundedRect(target.x, target.y, target.width, target.height, 1, LyraGridLayout::kCornerRadius, true);
  drawLucideIcon(renderer, icon_book_open_32, target.x + (target.width - kMissingIconSize) / 2,
                 target.y + (target.height - kMissingIconSize) / 2);
}
}  // namespace

void LyraGridTheme::setTileProgress(const int index, const float progressPercent) {
  if (index >= 0 && index < kTileCount) {
    gTileProgress[index] = progressPercent;
  }
}

void LyraGridTheme::clearTileProgress() { gTileProgress.fill(-1.0f); }

void LyraGridTheme::drawRecentBookCover(GfxRenderer& renderer, Rect rect, const std::vector<RecentBook>& recentBooks,
                                        int selectorIndex, bool& coverRendered, bool& coverBufferStored,
                                        bool& bufferRestored, const std::function<bool()>& storeCoverBuffer,
                                        const BookReadingStats* /*stats*/, float /*progressPercent*/,
                                        const GlobalReadingStats* /*globalStats*/,
                                        const char* /*currentChapterTitle*/) const {
  if (recentBooks.empty()) {
    drawEmptyRecents(renderer, rect);
    return;
  }

  const LyraGridLayout::Grid grid =
      LyraGridLayout::compute(renderer, rect, LyraCarouselTheme::buttonMenuTouchRect(renderer, 1).y);
  const int count = std::min(static_cast<int>(recentBooks.size()), kTileCount);

  // Covers, badges, bars and titles live in the cover snapshot; only the selection is drawn per frame.
  if (!coverRendered || !bufferRestored) {
    for (int i = 0; i < count; ++i) {
      const RecentBook& book = recentBooks[i];
      const Rect tile = LyraGridLayout::tileRect(grid, i);
      const Rect cover = LyraGridLayout::coverRect(grid, tile);
      if (!drawCoverBitmap(renderer, book, cover)) {
        drawMissingCover(renderer, cover);
      }
      if (book.pinSequence > 0) {
        LyraGridLayout::drawPinBadge(renderer, cover);
      }
      LyraGridLayout::drawProgressBar(renderer, cover, gTileProgress[i]);
      LyraGridLayout::drawTitle(renderer, tile, cover, book.title.empty() ? book.path : book.title);
    }
    coverBufferStored = storeCoverBuffer();
    coverRendered = coverBufferStored;
  }

  for (int i = 0; i < count; ++i) {
    const Rect tile = LyraGridLayout::tileRect(grid, i);
    TouchRegistry::getInstance().add(tile, i, TouchRegistry::Cover);
    if (i == selectorIndex) {
      LyraGridLayout::drawSelection(renderer, LyraGridLayout::coverRect(grid, tile));
    }
  }
}
