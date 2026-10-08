#pragma once

#include <BoardConfig.h>
#include <HalDisplay.h>
#include <HalGPIO.h>

#include "FluiDezSettings.h"
#include "util/QuickLockTrigger.h"

// X4's vendor FULL waveform visibly inverts the whole panel several times.
// Keep its manual shortcut on the clean HALF waveform used before the manual
// refresh change; X3 and every other device retain the explicit full refresh.
inline HalDisplay::RefreshMode manualScreenRefreshMode() {
#if FREEINK_DEVICE_X4
  if (gpio.deviceIsX4()) {
    return HalDisplay::HALF_REFRESH;
  }
#endif
  return HalDisplay::FULL_REFRESH;
}

inline bool isPowerButtonActionAvailableOutsideReader(const FluiDezSettings::SHORT_PWRBTN action) {
  switch (action) {
    case FluiDezSettings::SHORT_PWRBTN::SLEEP:
    case FluiDezSettings::SHORT_PWRBTN::SLEEP_ONLY:
    case FluiDezSettings::SHORT_PWRBTN::WAKE_ONLY:
    case FluiDezSettings::SHORT_PWRBTN::QUICK_LOCK:
    case FluiDezSettings::SHORT_PWRBTN::FORCE_REFRESH:
    case FluiDezSettings::SHORT_PWRBTN::SYNC_PROGRESS:
    case FluiDezSettings::SHORT_PWRBTN::SCREENSHOT:
    case FluiDezSettings::SHORT_PWRBTN::FILE_TRANSFER:
    case FluiDezSettings::SHORT_PWRBTN::CALIBRE_WIRELESS:
    case FluiDezSettings::SHORT_PWRBTN::JOIN_NETWORK:
    case FluiDezSettings::SHORT_PWRBTN::CREATE_HOTSPOT:
    case FluiDezSettings::SHORT_PWRBTN::TOGGLE_FRONTLIGHT:
    case FluiDezSettings::SHORT_PWRBTN::TOGGLE_TOUCHSCREEN:
    case FluiDezSettings::SHORT_PWRBTN::LIBRARY:
    case FluiDezSettings::SHORT_PWRBTN::HOME_READER:
      return true;
    case FluiDezSettings::SHORT_PWRBTN::IGNORE:
    case FluiDezSettings::SHORT_PWRBTN::PAGE_TURN:
    case FluiDezSettings::SHORT_PWRBTN::TOGGLE_FONT:
    case FluiDezSettings::SHORT_PWRBTN::TOGGLE_GUIDE_DOTS:
    case FluiDezSettings::SHORT_PWRBTN::TOGGLE_FOCUS_READING:
    case FluiDezSettings::SHORT_PWRBTN::TOGGLE_BOOKMARK:
    case FluiDezSettings::SHORT_PWRBTN::MARK_FINISHED:
    case FluiDezSettings::SHORT_PWRBTN::READING_STATS:
    case FluiDezSettings::SHORT_PWRBTN::CYCLE_PAGE_TURN:
    case FluiDezSettings::SHORT_PWRBTN::TOGGLE_TILT_PAGE_TURN:
    case FluiDezSettings::SHORT_PWRBTN::TOGGLE_DARK_MODE:
    case FluiDezSettings::SHORT_PWRBTN::FOOTNOTES:
    case FluiDezSettings::SHORT_PWRBTN::FILE_BROWSER:
    case FluiDezSettings::SHORT_PWRBTN::CREATE_CLIPPING:
    case FluiDezSettings::SHORT_PWRBTN::LOOKUP_WORD:
    case FluiDezSettings::SHORT_PWRBTN::TOGGLE_HOME_BUTTON_IN_READER:
    case FluiDezSettings::SHORT_PWRBTN::QUICK_ACTIONS:
    case FluiDezSettings::SHORT_PWRBTN::SHORT_PWRBTN_COUNT:
    default:
      return false;
  }
}

void enterDeepSleep(bool fromTimeout = false);
bool handleGlobalPowerButtonAction(FluiDezSettings::SHORT_PWRBTN action,
                                   QuickLockTrigger quickLockTrigger = QuickLockTrigger::None);
bool dispatchShortcutAction(FluiDezSettings::SHORT_PWRBTN action);
bool startGlobalSyncProgress(bool networkBootReady = false,
                             uint8_t readerOrientation = FluiDezSettings::ORIENTATION_COUNT);
