#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "components/themes/BaseTheme.h"

class GfxRenderer;

namespace DashboardLayout {

constexpr int kMaxCoverWidth = 296;
constexpr int kMaxCoverHeight = 444;

struct StatRow {
  std::string value;
  std::string label;
};

struct FooterIconItem {
  const uint8_t* icon = nullptr;
  std::string label;
};

struct FooterStatItem {
  std::string value;
  std::string label;
};

struct Content {
  std::string title;
  std::string subtitle;
  std::vector<StatRow> stats;
  std::vector<FooterIconItem> footerIcons;
  std::vector<FooterStatItem> footerStats;
};

struct TextLine {
  Rect rect;
  int fontId = 0;
  bool bold = false;
  std::string text;
};

struct StatLine {
  TextLine value;
  std::vector<TextLine> labels;
};

struct FooterIconLayout {
  Rect iconRect;
  TextLine label;
  const uint8_t* icon = nullptr;
};

struct FooterStatLayout {
  TextLine value;
  TextLine label;
};

struct Result {
  Rect coverRect = Rect{};
  Rect statsRect = Rect{};
  Rect textRect = Rect{};
  Rect footerRect = Rect{};
  int statsLabelFontId = 0;
  std::vector<StatLine> stats;
  std::vector<TextLine> titleLines;
  std::vector<TextLine> subtitleLines;
  std::vector<FooterIconLayout> footerIcons;
  std::vector<FooterStatLayout> footerStats;
};

Result compute(const GfxRenderer& renderer, const Rect& rect, const Content& content, bool isX3, int buttonHintReserve);
void draw(const GfxRenderer& renderer, const Result& layout, bool black = true, bool invertedIcons = false);

}  // namespace DashboardLayout
