#pragma once

// Keep input-only and Quick Lock returns on the same pacing policy as normal frames.
class MainLoopPacing {
  const unsigned long& lastActivityTime;
  bool skipDelay = false;

 public:
  explicit MainLoopPacing(const unsigned long& lastActivityTime) : lastActivityTime(lastActivityTime) {}
  ~MainLoopPacing();

  MainLoopPacing(const MainLoopPacing&) = delete;
  MainLoopPacing& operator=(const MainLoopPacing&) = delete;
  MainLoopPacing(MainLoopPacing&&) = delete;
  MainLoopPacing& operator=(MainLoopPacing&&) = delete;

  void setSkipDelay(bool skip) { skipDelay = skip; }
};
