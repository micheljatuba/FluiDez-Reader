#pragma once

#include <array>

#include "activities/reader/BookReadingStats.h"
#include "components/themes/lyra/LyraTheme.h"

class GfxRenderer;

// FluiDez native theme family. Lists, popups and keyboards reuse Lyra's tuned
// paths; Home is FluiDez's own, in three layouts that differ only in how much
// they read from the SD card:
//   Fluxo   – typographic, no image reads at all (fastest Home);
//   Cartões – one cached cover card plus small recents;
//   Estante – one cover plus a shelf of text spines.
namespace FluiDezMetrics {
constexpr ThemeMetrics makeBase() {
  ThemeMetrics v = LyraMetrics::values;
  v.listRowRadius = 10;
  v.headerUnderlineSize = 2;
  v.popupCornerRadius = 12;
  v.optionPopupSelectionRadius = 10;
  v.homeTopPadding = 52;
  v.homeMenuTopOffset = 8;
  return v;
}

constexpr ThemeMetrics makeFluxo() {
  ThemeMetrics v = makeBase();
  v.homeCoverHeight = LyraMetrics::values.homeCoverHeight;  // unused: Fluxo reads no covers
  v.homeCoverTileHeight = 600;
  v.homeRecentBooksCount = 6;
  return v;
}

constexpr ThemeMetrics makeCards() {
  ThemeMetrics v = makeBase();
  // Thumbnail height generated for the hero card; recents downscale it.
  v.homeCoverHeight = 186;
  v.homeCoverTileHeight = 560;
  v.homeRecentBooksCount = 4;
  return v;
}

constexpr ThemeMetrics makeShelf() {
  ThemeMetrics v = makeBase();
  v.homeCoverHeight = 196;
  v.homeCoverTileHeight = 600;
  v.homeRecentBooksCount = 6;
  return v;
}

constexpr ThemeMetrics base = makeBase();
constexpr ThemeMetrics fluxo = makeFluxo();
constexpr ThemeMetrics cards = makeCards();
constexpr ThemeMetrics shelf = makeShelf();
}  // namespace FluiDezMetrics

class FluiDezTheme : public LyraTheme {
 public:
  static constexpr int kMaxHomeBooks = 6;

  // Home publishes each visible book's progress (negative = unknown) and the
  // lead book's stats before rendering, so layouts never read the SD card.
  static void setHomeBookProgress(int index, float progressPercent);
  static void setLeadBookStats(const BookReadingStats& stats);
  static void clearHomeData();

 protected:
  static float homeBookProgress(int index);
  static const BookReadingStats& leadBookStats();

  // Draws a cached thumbnail fitted and centred in rect. Returns false when no
  // thumbnail is available so the caller can paint a typographic placeholder.
  static bool drawCoverFitted(const GfxRenderer& renderer, const RecentBook& book, Rect rect, int thumbHeight,
                              int cornerRadius);
  static void drawCoverPlaceholder(const GfxRenderer& renderer, const RecentBook& book, Rect rect, int cornerRadius);
  static void drawSectionLabel(const GfxRenderer& renderer, int x, int y, const char* label);
  static void drawProgressLine(const GfxRenderer& renderer, int x, int y, int width, int height, float percent,
                               int radius = 0);
  static bool estimateTimeLeft(const BookReadingStats& stats, float progressPercent, uint32_t& seconds);
  static void formatShortDuration(uint32_t seconds, char* buf, size_t len);
  static const char* bookTitle(const RecentBook& book);

  enum class DockStyle : uint8_t { Underline, Circle, Tile };
  // Bottom row of icon buttons shared by the three layouts; the focused item's
  // full label is drawn above it so long translations are never cut.
  void drawIconDock(const GfxRenderer& renderer, Rect rect, int buttonCount, int selectedIndex,
                    const std::function<const char*(int index)>& buttonLabel,
                    const std::function<UIIcon(int index)>& rowIcon, DockStyle style) const;
};

class FluiDezFluxoTheme : public FluiDezTheme {
 public:
  void drawRecentBookCover(GfxRenderer& renderer, Rect rect, const std::vector<RecentBook>& recentBooks,
                           int selectorIndex, bool& coverRendered, bool& coverBufferStored, bool& bufferRestored,
                           const std::function<bool()>& storeCoverBuffer, const BookReadingStats* stats = nullptr,
                           float progressPercent = -1.0f, const GlobalReadingStats* globalStats = nullptr,
                           const char* currentChapterTitle = nullptr) const override;
  void drawButtonMenu(GfxRenderer& renderer, Rect rect, int buttonCount, int selectedIndex,
                      const std::function<const char*(int index)>& buttonLabel,
                      const std::function<UIIcon(int index)>& rowIcon) const override;
  int homeCoverBookLimit() const override { return 0; }
};

class FluiDezCardsTheme : public FluiDezTheme {
 public:
  void drawRecentBookCover(GfxRenderer& renderer, Rect rect, const std::vector<RecentBook>& recentBooks,
                           int selectorIndex, bool& coverRendered, bool& coverBufferStored, bool& bufferRestored,
                           const std::function<bool()>& storeCoverBuffer, const BookReadingStats* stats = nullptr,
                           float progressPercent = -1.0f, const GlobalReadingStats* globalStats = nullptr,
                           const char* currentChapterTitle = nullptr) const override;
  void drawButtonMenu(GfxRenderer& renderer, Rect rect, int buttonCount, int selectedIndex,
                      const std::function<const char*(int index)>& buttonLabel,
                      const std::function<UIIcon(int index)>& rowIcon) const override;
};

class FluiDezShelfTheme : public FluiDezTheme {
 public:
  void drawRecentBookCover(GfxRenderer& renderer, Rect rect, const std::vector<RecentBook>& recentBooks,
                           int selectorIndex, bool& coverRendered, bool& coverBufferStored, bool& bufferRestored,
                           const std::function<bool()>& storeCoverBuffer, const BookReadingStats* stats = nullptr,
                           float progressPercent = -1.0f, const GlobalReadingStats* globalStats = nullptr,
                           const char* currentChapterTitle = nullptr) const override;
  void drawButtonMenu(GfxRenderer& renderer, Rect rect, int buttonCount, int selectedIndex,
                      const std::function<const char*(int index)>& buttonLabel,
                      const std::function<UIIcon(int index)>& rowIcon) const override;
  int homeCoverBookLimit() const override { return 1; }
};
