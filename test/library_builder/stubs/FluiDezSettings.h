#pragma once

#include <cstdint>

struct FluiDezSettings {
  uint8_t trackReadingStats = 1;
  bool shouldTrackReadingStats() const { return trackReadingStats != 0; }
};

inline FluiDezSettings SETTINGS;
