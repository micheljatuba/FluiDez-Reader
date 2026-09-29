#pragma once

#include <array>
#include <cstdint>

class HalGPIO {
 public:
  enum Button : uint8_t { BTN_POWER, BTN_DOWN, BTN_UP };

  std::array<bool, 3> pressed{};
  bool inputReceived = false;
  bool touch = true;
  bool rawContact = false;
  bool simulatorSleepRequested = false;
  mutable unsigned rawPolls = 0;

  bool wasAnyPressed() const { return inputReceived; }
  bool wasAnyReleased() const { return false; }
  bool wasTouchActivity() const { return false; }
  bool wasReleased(uint8_t) const { return false; }
  bool isPressed(uint8_t button) const { return pressed[button]; }
  unsigned long getPowerButtonHeldTime() const { return 0; }
  bool hasTouch() const { return touch; }
  bool wasUsbStateChanged() const { return false; }
  bool isUsbConnected() const { return false; }
  bool isUsbConnectedCached() const { return false; }
  bool rawInputActive() const {
    ++rawPolls;
    return rawContact || pressed[BTN_POWER] || pressed[BTN_DOWN] || pressed[BTN_UP];
  }
  bool consumeSimulatorSleepRequest() {
    const bool requested = simulatorSleepRequested;
    simulatorSleepRequested = false;
    return requested;
  }
};

extern HalGPIO gpio;
