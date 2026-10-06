#include <Arduino.h>
#include <HalGPIO.h>
#include <HalPowerManager.h>
#include <gtest/gtest.h>

#include <limits>
#include <vector>

#include "ButtonShortcutController.h"
#include "MainLoopPacing.h"

namespace {
unsigned long clockMs = 0;
std::vector<unsigned long> delays;
unsigned yields = 0;
unsigned renderLocks = 0;
unsigned delaysUnderRenderLock = 0;
// When set, RenderLock(Mode::Try) fails as if the render task held the lock.
bool renderTaskBusy = false;
unsigned sleeps = 0;
unsigned screenshots = 0;
bool homeGesturePending = false;
}  // namespace

HalGPIO gpio;
HalPowerManager powerManager;

unsigned long millis() { return clockMs; }
void delay(const unsigned long milliseconds) {
  delays.push_back(milliseconds);
  clockMs += milliseconds;
  if (renderLocks > 0) ++delaysUnderRenderLock;
}
void yield() { ++yields; }

namespace {
struct CrossPointSettings {
  enum class SHORT_PWRBTN { NONE, QUICK_LOCK };
  static constexpr int IGNORE = 0;
};

struct MappedInputManager {
  enum class Button { Back, Confirm, Left, Right, Up, Down, Power, PageBack, PageForward };
};

namespace ReaderUtils {
constexpr unsigned long SKIP_HOLD_MS = 700;
}  // namespace ReaderUtils

struct Settings {
  bool tiltPageTurn = false;
  int tiltPageTurnDirection = 0;
  int orientation = 0;
  bool fadingFix = false;
  bool disableReaderTouchscreen = false;
  int sideButtonUpLong = CrossPointSettings::IGNORE;
  int sideButtonDownLong = CrossPointSettings::IGNORE;
  CrossPointSettings::SHORT_PWRBTN shortPwrBtn = CrossPointSettings::SHORT_PWRBTN::NONE;
  unsigned long sleepTimeoutMs = 0;

  unsigned long getPowerButtonLongPressDuration() const { return 600; }
  unsigned long getSleepTimeoutMs() const { return sleepTimeoutMs; }
} SETTINGS;

struct {
  bool quickLockResumePending = false;
} APP_STATE;

struct {
  void setFadingFix(bool) {}
} renderer;

struct {
  void update(bool, int, int, bool) {}
  bool hadActivity() const { return false; }
} halTiltSensor;

struct {
  unsigned inputPolls = 0;
  unsigned releaseClears = 0;
  void update() { ++inputPolls; }
  void clearInjectedReleases() { ++releaseClears; }
  void clearDeferredHomeGesture() {}
  bool isPressed(MappedInputManager::Button) const { return false; }
  bool wasPressed(MappedInputManager::Button) const { return false; }
  bool wasReleased(MappedInputManager::Button) const { return false; }
} mappedInputManager;

struct {
  bool exclusiveStorage = false;
  bool preventSleep = false;
  bool fastPolling = false;
  bool modal = false;
  unsigned long workMs = 0;
  bool busyAfterLoop = false;
  unsigned dispatches = 0;
  unsigned inputNotifications = 0;

  bool requiresExclusiveStorageLoop() const { return exclusiveStorage; }
  bool preventAutoSleep() const { return preventSleep; }
  bool isReaderActivity() const { return true; }
  bool isHomeActivity() const { return false; }
  bool blocksGlobalInput() const { return modal; }
  bool handleQuickLockUnlock(QuickLockTrigger) const { return false; }
  bool skipLoopDelay() const { return fastPolling; }
  uint8_t pollDelayMs = 10;
  uint8_t inputPollDelayMs() const { return pollDelayMs; }
  void notifyUserInput() { ++inputNotifications; }
  void requestUpdate() {}
  void loop() {
    ++dispatches;
    clockMs += workMs;
    if (busyAfterLoop) preventSleep = true;
  }
} activityManager;

struct {
  uint32_t getBufferSize() const { return 0; }
  uint8_t* getFrameBuffer() const { return nullptr; }
} display;

constexpr bool Serial = false;
// BoardConfig log transports; the tests use the serial one so periodic memory logs stay off.
#define FREEINK_LOG_TRANSPORT_SERIAL 0
#define FREEINK_LOG_TRANSPORT_ROM_PRINTF 2
#define FREEINK_LOG_TRANSPORT FREEINK_LOG_TRANSPORT_SERIAL
struct {
  void printf(const char*, ...) {}
  void write(const uint8_t*, uint32_t) {}
} logSerial;

class RenderLock {
 public:
  enum class Mode { Blocking, Try };
  explicit RenderLock(Mode mode = Mode::Blocking) : owns(mode == Mode::Blocking || !renderTaskBusy) {
    if (owns) ++renderLocks;
  }
  ~RenderLock() {
    if (owns) --renderLocks;
  }
  bool ownsLock() const { return owns; }

 private:
  bool owns;
};

namespace ScreenshotUtil {
template <typename Renderer>
void takeScreenshot(Renderer&) {
  ++screenshots;
}
}  // namespace ScreenshotUtil

namespace UsbSerialFileTransfer {
enum class ProcessResult { None, ScreenshotRequested };
ProcessResult process(bool) { return ProcessResult::None; }
}  // namespace UsbSerialFileTransfer

ButtonShortcutController buttonShortcutController;
unsigned long allowSleepAt = 0;
bool powerButtonReleasedSinceWake = true;
bool wakePowerReleasePending = false;

void logMemoryStats(const char*) {}
void notifyQuickLockChanged() {}
void enterDeepSleep(bool = false) { ++sleeps; }
bool handleX4ProHomeKeyQuickLockUnlock() { return false; }
bool handleX4ProHomeKeyShortcuts() { return homeGesturePending; }
bool dispatchButtonShortcut(const ButtonShortcutController::Result& result) {
  return result.event != ButtonShortcutController::Event::None;
}
ButtonShortcutController::ChordAction configuredSideButtonChordAction() {
  return ButtonShortcutController::ChordAction::Disabled;
}
ButtonShortcutController::ChordAction configuredChordAction() {
  return ButtonShortcutController::ChordAction::Disabled;
}
CrossPointSettings::SHORT_PWRBTN getPowerButtonAction() { return CrossPointSettings::SHORT_PWRBTN::NONE; }
bool dispatchShortcutAction(CrossPointSettings::SHORT_PWRBTN) { return false; }
bool handleGlobalPowerButtonAction(CrossPointSettings::SHORT_PWRBTN, QuickLockTrigger) { return false; }

#ifdef SIMULATOR
struct {
  void update() {}
} simulatorHomeKeyInput;
void runSimulatorSmokeTestTick() {}
#endif

#define LOG_DBG(...) ((void)0)
// CMake copies the production loop verbatim; only its hardware/activity boundaries are stubbed.
#include "MainLoopUnderTest.inc"
#undef LOG_DBG

class MainLoopPacingTest : public testing::Test {
 protected:
  unsigned long lastInputMs = 0;

  void clearObservations() {
    delays.clear();
    yields = 0;
    gpio.rawPolls = 0;
    powerManager.requests.clear();
    activityManager.dispatches = 0;
    mappedInputManager.inputPolls = 0;
    mappedInputManager.releaseClears = 0;
    screenshots = 0;
    sleeps = 0;
    delaysUnderRenderLock = 0;
  }

  void recordInput() {
    gpio.inputReceived = true;
    lastInputMs = clockMs;
    loop();
    gpio.inputReceived = false;
    clearObservations();
  }

  void SetUp() override {
    clockMs = 10000;
    gpio = HalGPIO{};
    activityManager = {};
    SETTINGS = {};
    APP_STATE = {};
    buttonShortcutController = {};
    homeGesturePending = false;
    powerButtonReleasedSinceWake = true;
    wakePowerReleasePending = false;
    renderLocks = 0;
    renderTaskBusy = false;
    recordInput();
  }

  void becomeIdle() { clockMs = lastInputMs + HalPowerManager::IDLE_POWER_SAVING_MS; }

  static unsigned long idleBudget(bool touch) {
#ifdef SIMULATOR
    (void)touch;
    return 50;
#else
    return touch ? 20 : 50;
#endif
  }
};

TEST_F(MainLoopPacingTest, ActiveFrameKeepsTenMillisecondDelay) {
  const auto start = clockMs;
  loop();
  EXPECT_EQ(clockMs - start, 10UL);
  EXPECT_EQ(delays, std::vector<unsigned long>({10}));
  EXPECT_TRUE(powerManager.requests.empty());
  EXPECT_EQ(activityManager.dispatches, 1U);
}

TEST_F(MainLoopPacingTest, ActiveFrameUsesTheActivityPollInterval) {
  // Text entry asks for faster polling (Activity::inputPollDelayMs()).
  activityManager.pollDelayMs = 2;
  loop();
  EXPECT_EQ(delays, std::vector<unsigned long>({2}));
  EXPECT_EQ(activityManager.dispatches, 1U);
}

TEST_F(MainLoopPacingTest, BusyRenderTaskWaitsOnePollIntervalWithoutPowerSaving) {
  // While a page is drawn the loop keeps reading input at the activity's rate
  // instead of blocking on the render lock or entering the idle budget.
  becomeIdle();
  activityManager.pollDelayMs = 2;
  renderTaskBusy = true;
  loop();
  EXPECT_EQ(delays, std::vector<unsigned long>({2}));
  EXPECT_EQ(yields, 0U);
  EXPECT_EQ(delaysUnderRenderLock, 0U);
}

TEST_F(MainLoopPacingTest, IdleTouchFrameKeepsExistingBudget) {
  becomeIdle();
  const auto start = clockMs;
  loop();
  EXPECT_EQ(clockMs - start, idleBudget(true));
  EXPECT_EQ(powerManager.requests, std::vector<bool>({true}));
  EXPECT_EQ(yields, 0U);
}

TEST_F(MainLoopPacingTest, IdleButtonFrameKeepsFiftyMillisecondBudget) {
  gpio.touch = false;
  becomeIdle();
  const auto start = clockMs;
  loop();
  EXPECT_EQ(clockMs - start, 50UL);
  EXPECT_EQ(powerManager.requests, std::vector<bool>({true}));
}

TEST_F(MainLoopPacingTest, ActiveWorkKeepsFastPolling) {
  becomeIdle();
  activityManager.fastPolling = true;
  loop();
  EXPECT_TRUE(delays.empty());
  EXPECT_EQ(powerManager.requests, std::vector<bool>({false}));
  EXPECT_EQ(yields, 1U);
  EXPECT_EQ(activityManager.dispatches, 1U);
}

TEST_F(MainLoopPacingTest, QuickLockDoesNotBypassIdlePowerSaving) {
  buttonShortcutController.toggleQuickLock(clockMs, QuickLockTrigger::ShortPower);
  becomeIdle();
  const auto start = clockMs;
  loop();
  EXPECT_EQ(clockMs - start, idleBudget(true));
  EXPECT_EQ(powerManager.requests, std::vector<bool>({true}));
  EXPECT_EQ(activityManager.dispatches, 0U);
  EXPECT_EQ(mappedInputManager.releaseClears, 1U);
  EXPECT_TRUE(buttonShortcutController.isQuickLocked());
}

TEST_F(MainLoopPacingTest, QuickLockDoesNotSpinForSuspendedFastPollingActivity) {
  buttonShortcutController.toggleQuickLock(clockMs, QuickLockTrigger::ShortPower);
  activityManager.fastPolling = true;
  becomeIdle();
  loop();
  EXPECT_FALSE(delays.empty());
  EXPECT_EQ(powerManager.requests, std::vector<bool>({true}));
  EXPECT_EQ(yields, 0U);
  EXPECT_EQ(activityManager.dispatches, 0U);
}

TEST_F(MainLoopPacingTest, HeldScreenshotChordYieldsAfterReleasingRenderLock) {
  gpio.pressed[HalGPIO::BTN_POWER] = true;
  gpio.pressed[HalGPIO::BTN_DOWN] = true;
  loop();
  EXPECT_EQ(screenshots, 1U);
  EXPECT_EQ(delays, std::vector<unsigned long>({10}));
  EXPECT_EQ(delaysUnderRenderLock, 0U);
  clearObservations();
  loop();
  EXPECT_EQ(screenshots, 0U);
  EXPECT_EQ(delays, std::vector<unsigned long>({10}));
}

TEST_F(MainLoopPacingTest, PendingHomeGestureDoesNotSpin) {
  homeGesturePending = true;
  loop();
  EXPECT_EQ(delays, std::vector<unsigned long>({10}));
  EXPECT_EQ(activityManager.dispatches, 0U);
}

TEST_F(MainLoopPacingTest, WakeReleaseSuppressionStillYields) {
  wakePowerReleasePending = true;
  loop();
  EXPECT_FALSE(wakePowerReleasePending);
  EXPECT_EQ(delays, std::vector<unsigned long>({10}));
  EXPECT_EQ(activityManager.dispatches, 0U);
}

TEST_F(MainLoopPacingTest, NewInputUsesUpdatedInactivityTimestamp) {
  becomeIdle();
  gpio.inputReceived = true;
  loop();
  EXPECT_EQ(delays, std::vector<unsigned long>({10}));
  EXPECT_EQ(powerManager.requests, std::vector<bool>({false}));
}

TEST_F(MainLoopPacingTest, ExclusiveStorageRetainsItsOwnActiveDelay) {
  activityManager.exclusiveStorage = true;
  activityManager.preventSleep = true;
  loop();
  EXPECT_EQ(delays, std::vector<unsigned long>({10}));
  EXPECT_EQ(powerManager.requests, std::vector<bool>({false}));
  EXPECT_EQ(activityManager.dispatches, 1U);
}

TEST_F(MainLoopPacingTest, ExclusiveStorageRetainsItsOwnIdleDelay) {
  activityManager.exclusiveStorage = true;
  loop();
  EXPECT_EQ(delays, std::vector<unsigned long>({50}));
  EXPECT_EQ(powerManager.requests, std::vector<bool>({true}));
  EXPECT_EQ(activityManager.dispatches, 1U);
}

TEST_F(MainLoopPacingTest, IdleBudgetHandlesClockWraparound) {
  clockMs = std::numeric_limits<unsigned long>::max() - 5000;
  recordInput();
  clockMs = std::numeric_limits<unsigned long>::max() - 5;
  const auto start = clockMs;
  loop();
  EXPECT_EQ(clockMs - start, idleBudget(true));
  EXPECT_EQ(powerManager.requests, std::vector<bool>({true}));
}

TEST_F(MainLoopPacingTest, UserStartedWorkRestartsIdleCountdownWhenItFinishes) {
  SETTINGS.sleepTimeoutMs = 60000;
  activityManager.workMs = 5 * 60000;
  gpio.inputReceived = true;
  loop();
  gpio.inputReceived = false;
  activityManager.workMs = 0;
  clearObservations();

  loop();
  EXPECT_EQ(sleeps, 0U);
  EXPECT_EQ(delays, std::vector<unsigned long>({10}));
  EXPECT_TRUE(powerManager.requests.empty());

  clockMs += 59000;
  loop();
  EXPECT_EQ(sleeps, 0U);
  clockMs += 1000;
  loop();
  EXPECT_EQ(sleeps, 1U);
}

TEST_F(MainLoopPacingTest, BusyFrameRestartsIdleCountdownWhenItFinishes) {
  SETTINGS.sleepTimeoutMs = 60000;
  activityManager.preventSleep = true;
  activityManager.workMs = 5 * 60000;
  loop();
  activityManager.preventSleep = false;
  activityManager.workMs = 0;
  clearObservations();

  loop();
  EXPECT_EQ(sleeps, 0U);
  EXPECT_EQ(delays, std::vector<unsigned long>({10}));
}

TEST_F(MainLoopPacingTest, WorkThatBecomesBusyRestartsIdleCountdown) {
  SETTINGS.sleepTimeoutMs = 60000;
  clockMs = lastInputMs + 50000;
  activityManager.workMs = 20000;
  activityManager.busyAfterLoop = true;
  loop();
  activityManager.workMs = 0;
  activityManager.busyAfterLoop = false;
  activityManager.preventSleep = false;
  clearObservations();

  loop();
  EXPECT_EQ(sleeps, 0U);
  EXPECT_EQ(delays, std::vector<unsigned long>({10}));
}

TEST_F(MainLoopPacingTest, SlowIdleFrameDoesNotPostponeSleep) {
  SETTINGS.sleepTimeoutMs = 60000;
  clockMs = lastInputMs + 59000;
  activityManager.workMs = 2000;
  loop();
  EXPECT_EQ(sleeps, 0U);
  activityManager.workMs = 0;
  clearObservations();

  loop();
  EXPECT_EQ(sleeps, 1U);
}

#ifndef SIMULATOR
TEST_F(MainLoopPacingTest, RawContactEndsIdleWaitAfterFirstSlice) {
  becomeIdle();
  gpio.rawContact = true;
  loop();
  EXPECT_EQ(delays, std::vector<unsigned long>({10}));
  EXPECT_EQ(gpio.rawPolls, 1U);
}
#else
TEST_F(MainLoopPacingTest, SimulatorSleepUsesUpdatedInactivityTimestamp) {
  becomeIdle();
  gpio.simulatorSleepRequested = true;
  loop();
  EXPECT_EQ(sleeps, 1U);
  EXPECT_EQ(delays, std::vector<unsigned long>({10}));
}
#endif
}  // namespace
