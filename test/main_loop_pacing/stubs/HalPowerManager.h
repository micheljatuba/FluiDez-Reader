#pragma once

#include <cstdint>
#include <vector>

class HalPowerManager {
 public:
  static constexpr unsigned long IDLE_POWER_SAVING_MS = 3000;
  std::vector<bool> requests;

  void setPowerSaving(bool enabled) { requests.push_back(enabled); }
  uint16_t getBatteryPercentage() const { return 50; }
};

extern HalPowerManager powerManager;
