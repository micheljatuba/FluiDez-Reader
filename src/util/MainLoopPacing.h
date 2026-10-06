#pragma once

#include <cstdint>

// Keep input-only and Quick Lock returns on the same pacing policy as normal frames.
class MainLoopPacing {
  const unsigned long& lastActivityTime;
  bool skipDelay = false;
  bool renderBusy = false;
  uint8_t inputPollDelayMs = 10;

 public:
  explicit MainLoopPacing(const unsigned long& lastActivityTime) : lastActivityTime(lastActivityTime) {}
  ~MainLoopPacing();

  MainLoopPacing(const MainLoopPacing&) = delete;
  MainLoopPacing& operator=(const MainLoopPacing&) = delete;
  MainLoopPacing(MainLoopPacing&&) = delete;
  MainLoopPacing& operator=(MainLoopPacing&&) = delete;

  void setSkipDelay(bool skip) { skipDelay = skip; }
  // Per-activity active-input poll interval (Activity::inputPollDelayMs()).
  void setInputPollDelayMs(uint8_t delayMs) { inputPollDelayMs = delayMs; }
  // The render task holds the render lock: wait one poll interval and keep reading input.
  void setRenderBusy() { renderBusy = true; }
};
