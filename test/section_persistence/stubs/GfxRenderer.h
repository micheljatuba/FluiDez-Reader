#pragma once

#include <cstdint>

#include "FontCacheManager.h"

class GfxRenderer {
 public:
  FontCacheManager* getFontCacheManager() const { return nullptr; }
  bool isSdCardFont(int) const { return false; }
  bool releaseSdCardFontForLowMemory(int, bool = false) { return false; }
};
