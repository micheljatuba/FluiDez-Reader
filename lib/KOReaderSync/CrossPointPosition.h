#pragma once

#include <cstdint>

/**
 * CrossPoint position representation.
 */
struct CrossPointPosition {
  int spineIndex;                  // Current spine item (chapter) index
  int pageNumber;                  // Current page within the spine item
  int totalPages;                  // Total pages in the current spine item
  uint16_t paragraphIndex = 0;     // 1-based synthetic paragraph index from XPath p[N]
  bool hasParagraphIndex = false;  // True when paragraphIndex was resolved from XPath
  uint16_t liIndex = 0;            // Running <li> count at the matched XPath element
  bool hasLiIndex = false;         // True when target element is <li> and liIndex was resolved
  char xpathAnchorId[64] = {};     // First <a id> captured inside the matched XPath element
  uint32_t visibleTextOffset = 0;  // Zero-based visible codepoint offset in the spine item
  bool hasVisibleTextOffset = false;
  bool valid = true;  // False when an exact requested coordinate mapping is unavailable
  // Trust flags for progress comparison (compareProgress): positions estimated
  // from a percentage must not outrank layout-backed ones.
  bool hasResolvedSpineIndex = false;  // Spine came from local state or an XPath DocFragment, not percentage
  bool hasMappedPage = false;          // Page came from local layout data, not percentage scaling
};
