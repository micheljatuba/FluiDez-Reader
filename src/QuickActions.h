#pragma once

#include <CrossInkHalFrontlight.h>
#include <HalGPIO.h>
#include <HalTiltSensor.h>
#include <I18n.h>

#include <array>
#include <functional>

#include "FluiDezSettings.h"

class OptionPopup;

// One source of truth for the shortcut that opens Quick Actions.  UI and web
// settings call this after changing an action, so the persisted state cannot
// end up with two physical gestures claiming the same menu.
namespace QuickActions {
// Keep existing values stable because the selected trigger is persisted.
enum class Trigger : uint8_t {
  None = 0,
  ShortPower,
  LongPower,
  LongBack,
  LongMenu,
  PowerUp,
  TapHome,
  LongPressHome,
  DoubleTapHome,
  UpDown,
  SideUpShort,
  SideUpLong,
  SideDownShort,
  SideDownLong,
};

inline constexpr std::array<StrId, FluiDezSettings::QUICK_ACTION_SLOT_ACTION_COUNT> actionLabels = {
    StrId::STR_IGNORE,
    StrId::STR_SLEEP,
    StrId::STR_PAGE_TURN,
    StrId::STR_FORCE_REFRESH,
    StrId::STR_CHANGE_FONT,
    StrId::STR_TOGGLE_GUIDE_DOTS,
    StrId::STR_TOGGLE_FOCUS_READING,
    StrId::STR_TOGGLE_BOOKMARK,
    StrId::STR_SYNC_PROGRESS,
    StrId::STR_MARK_FINISHED,
    StrId::STR_READING_STATS,
    StrId::STR_SCREENSHOT_BUTTON,
    StrId::STR_CYCLE_PAGE_TURN,
    StrId::STR_FILE_TRANSFER,
    StrId::STR_TILT_PAGE_TURN,
    StrId::STR_READER_DARK_MODE,
    StrId::STR_FOOTNOTES,
    StrId::STR_BROWSE_FILES,
    StrId::STR_CALIBRE_WIRELESS,
    StrId::STR_JOIN_NETWORK,
    StrId::STR_CREATE_HOTSPOT,
    StrId::STR_SAVE_CLIPPING,
    StrId::STR_LOOKUP};

// Shared display order for shortcut pickers. The values remain the persisted
// SHORT_PWRBTN IDs; only their presentation order is centralized here.
inline constexpr std::array<FluiDezSettings::SHORT_PWRBTN, 32> shortcutActionOrder = {
    FluiDezSettings::IGNORE,
    FluiDezSettings::SLEEP,
    FluiDezSettings::PAGE_TURN,
    FluiDezSettings::PREVIOUS_PAGE,
    FluiDezSettings::TOGGLE_BOOKMARK,
    FluiDezSettings::READING_STATS,
    FluiDezSettings::MARK_FINISHED,
    FluiDezSettings::FORCE_REFRESH,
    FluiDezSettings::TOGGLE_FONT,
    FluiDezSettings::TOGGLE_GUIDE_DOTS,
    FluiDezSettings::TOGGLE_FOCUS_READING,
    FluiDezSettings::CYCLE_PAGE_TURN,
    FluiDezSettings::TOGGLE_TILT_PAGE_TURN,
    FluiDezSettings::SYNC_PROGRESS,
    FluiDezSettings::NEARBY_POSITION_SYNC,
    FluiDezSettings::LIBRARY,
    FluiDezSettings::FILE_TRANSFER,
    FluiDezSettings::CALIBRE_WIRELESS,
    FluiDezSettings::JOIN_NETWORK,
    FluiDezSettings::CREATE_HOTSPOT,
    FluiDezSettings::SCREENSHOT,
    FluiDezSettings::TOGGLE_DARK_MODE,
    FluiDezSettings::FOOTNOTES,
    FluiDezSettings::FILE_BROWSER,
    FluiDezSettings::CREATE_CLIPPING,
    FluiDezSettings::LOOKUP_WORD,
    FluiDezSettings::TOGGLE_HOME_BUTTON_IN_READER,
    FluiDezSettings::QUICK_ACTIONS,
    FluiDezSettings::QUICK_LOCK,
    FluiDezSettings::TOGGLE_FRONTLIGHT,
    FluiDezSettings::TOGGLE_TOUCHSCREEN,
    FluiDezSettings::HOME_READER,
};

inline bool supportsTiltPageTurn() { return halTiltSensor.isAvailable(); }

inline bool isActionAvailable(const uint8_t action) {
  if (action == FluiDezSettings::READING_STATS && !SETTINGS.shouldTrackReadingStats()) return false;
  if (action == FluiDezSettings::PREVIOUS_PAGE || action == FluiDezSettings::NEARBY_POSITION_SYNC ||
      action == FluiDezSettings::LIBRARY)
    return true;
  if (action == FluiDezSettings::QUICK_ACTIONS || action == FluiDezSettings::QUICK_LOCK) return true;
  if (action == FluiDezSettings::TOGGLE_FRONTLIGHT) return Frontlight.present();
  if (action == FluiDezSettings::TOGGLE_TOUCHSCREEN) return gpio.hasTouch();
  if (action == FluiDezSettings::HOME_READER) return !gpio.hasTouch();
  if (action < FluiDezSettings::QUICK_ACTION_SLOT_ACTION_COUNT) {
    return action != FluiDezSettings::TOGGLE_TILT_PAGE_TURN || supportsTiltPageTurn();
  }
  return action == FluiDezSettings::TOGGLE_HOME_BUTTON_IN_READER && gpio.hasHomeKey();
}

// Quick Lock needs a single physical shortcut to unlock. Quick Actions opens
// this same menu, so neither belongs in a menu slot.
inline bool isQuickActionSlotActionAvailable(const uint8_t action) {
  return action != FluiDezSettings::QUICK_LOCK && action != FluiDezSettings::QUICK_ACTIONS && isActionAvailable(action);
}

inline StrId actionLabel(const uint8_t action) {
  // Use the directional label for the legacy page-turn action ID.
  if (action == FluiDezSettings::PAGE_TURN) return StrId::STR_NEXT_PAGE;
  if (action < FluiDezSettings::QUICK_ACTION_SLOT_ACTION_COUNT) return actionLabels[action];
  if (action == FluiDezSettings::QUICK_ACTIONS) return StrId::STR_QUICK_ACTIONS;
  if (action == FluiDezSettings::TOGGLE_FRONTLIGHT) return StrId::STR_TOGGLE_FRONTLIGHT;
  if (action == FluiDezSettings::TOGGLE_TOUCHSCREEN) return StrId::STR_TOGGLE_TOUCHSCREEN;
  if (action == FluiDezSettings::QUICK_LOCK) return StrId::STR_QUICK_LOCK;
  if (action == FluiDezSettings::PREVIOUS_PAGE) return StrId::STR_PREV_PAGE;
  if (action == FluiDezSettings::NEARBY_POSITION_SYNC) return StrId::STR_NEARBY_POSITION_SYNC;
  if (action == FluiDezSettings::LIBRARY) return StrId::STR_LIBRARY;
  if (action == FluiDezSettings::HOME_READER) return StrId::STR_HOME_READER;
  return StrId::STR_HOME_BUTTON_LOCK;
}

inline void synchronize(FluiDezSettings& settings, Trigger preferred = Trigger::None) {
  const bool shortPower = settings.shortPwrBtn == FluiDezSettings::QUICK_ACTIONS;
  const bool longPower = settings.longPwrBtn == FluiDezSettings::QUICK_ACTIONS;
  const bool longBack = settings.longPressBackAction == FluiDezSettings::LONG_MENU_QUICK_ACTIONS;
  const bool longMenu = settings.longPressMenuAction == FluiDezSettings::LONG_MENU_QUICK_ACTIONS;
  const bool powerUp = settings.powerChordAction == FluiDezSettings::CHORD_QUICK_ACTIONS;
  const bool upDown = settings.sideButtonChordAction == FluiDezSettings::CHORD_QUICK_ACTIONS;
  const bool tapHome = settings.homeButtonTapAction == FluiDezSettings::QUICK_ACTIONS;
  const bool longPressHome = settings.homeButtonLongPressAction == FluiDezSettings::QUICK_ACTIONS;
  const bool doubleTapHome = settings.homeButtonDoubleTapAction == FluiDezSettings::QUICK_ACTIONS;
  const bool sideUpShort = settings.sideButtonUpShort == FluiDezSettings::QUICK_ACTIONS;
  const bool sideUpLong = settings.sideButtonUpLong == FluiDezSettings::QUICK_ACTIONS;
  const bool sideDownShort = settings.sideButtonDownShort == FluiDezSettings::QUICK_ACTIONS;
  const bool sideDownLong = settings.sideButtonDownLong == FluiDezSettings::QUICK_ACTIONS;

  Trigger owner = preferred;
  if (owner == Trigger::None) {
    if (shortPower)
      owner = Trigger::ShortPower;
    else if (longPower)
      owner = Trigger::LongPower;
    else if (longBack)
      owner = Trigger::LongBack;
    else if (longMenu)
      owner = Trigger::LongMenu;
    else if (powerUp)
      owner = Trigger::PowerUp;
    else if (upDown)
      owner = Trigger::UpDown;
    else if (tapHome)
      owner = Trigger::TapHome;
    else if (longPressHome)
      owner = Trigger::LongPressHome;
    else if (doubleTapHome)
      owner = Trigger::DoubleTapHome;
    else if (sideUpShort)
      owner = Trigger::SideUpShort;
    else if (sideUpLong)
      owner = Trigger::SideUpLong;
    else if (sideDownShort)
      owner = Trigger::SideDownShort;
    else if (sideDownLong)
      owner = Trigger::SideDownLong;
  }

  if (owner != Trigger::ShortPower && shortPower) settings.shortPwrBtn = FluiDezSettings::IGNORE;
  if (owner != Trigger::LongPower && longPower) settings.longPwrBtn = FluiDezSettings::IGNORE;
  if (owner != Trigger::LongBack && longBack) settings.longPressBackAction = FluiDezSettings::LONG_MENU_OFF;
  if (owner != Trigger::LongMenu && longMenu) settings.longPressMenuAction = FluiDezSettings::LONG_MENU_OFF;
  if (owner != Trigger::PowerUp && powerUp) settings.powerChordAction = FluiDezSettings::CHORD_DISABLED;
  if (owner != Trigger::UpDown && upDown) settings.sideButtonChordAction = FluiDezSettings::CHORD_DISABLED;
  if (owner != Trigger::TapHome && tapHome) settings.homeButtonTapAction = FluiDezSettings::HOME_BUTTON_BACK_HOME;
  if (owner != Trigger::LongPressHome && longPressHome) {
    settings.homeButtonLongPressAction = FluiDezSettings::HOME_BUTTON_READER_MENU;
  }
  if (owner != Trigger::DoubleTapHome && doubleTapHome) {
    settings.homeButtonDoubleTapAction = FluiDezSettings::HOME_BUTTON_TOGGLE_FRONTLIGHT;
  }
  if (owner != Trigger::SideUpShort && sideUpShort) settings.sideButtonUpShort = FluiDezSettings::IGNORE;
  if (owner != Trigger::SideUpLong && sideUpLong) settings.sideButtonUpLong = FluiDezSettings::IGNORE;
  if (owner != Trigger::SideDownShort && sideDownShort) settings.sideButtonDownShort = FluiDezSettings::IGNORE;
  if (owner != Trigger::SideDownLong && sideDownLong) settings.sideButtonDownLong = FluiDezSettings::IGNORE;
  settings.quickActionsTrigger = static_cast<uint8_t>(owner);
}

inline void applyTrigger(FluiDezSettings& settings, const Trigger trigger) {
  if (settings.shortPwrBtn == FluiDezSettings::QUICK_ACTIONS) settings.shortPwrBtn = FluiDezSettings::IGNORE;
  if (settings.longPwrBtn == FluiDezSettings::QUICK_ACTIONS) settings.longPwrBtn = FluiDezSettings::IGNORE;
  if (settings.longPressBackAction == FluiDezSettings::LONG_MENU_QUICK_ACTIONS) {
    settings.longPressBackAction = FluiDezSettings::LONG_MENU_OFF;
  }
  if (settings.longPressMenuAction == FluiDezSettings::LONG_MENU_QUICK_ACTIONS) {
    settings.longPressMenuAction = FluiDezSettings::LONG_MENU_OFF;
  }
  if (settings.powerChordAction == FluiDezSettings::CHORD_QUICK_ACTIONS) {
    settings.powerChordAction = FluiDezSettings::CHORD_DISABLED;
  }
  if (settings.sideButtonChordAction == FluiDezSettings::CHORD_QUICK_ACTIONS) {
    settings.sideButtonChordAction = FluiDezSettings::CHORD_DISABLED;
  }
  if (settings.homeButtonTapAction == FluiDezSettings::QUICK_ACTIONS) {
    settings.homeButtonTapAction = FluiDezSettings::HOME_BUTTON_BACK_HOME;
  }
  if (settings.homeButtonLongPressAction == FluiDezSettings::QUICK_ACTIONS) {
    settings.homeButtonLongPressAction = FluiDezSettings::HOME_BUTTON_READER_MENU;
  }
  if (settings.homeButtonDoubleTapAction == FluiDezSettings::QUICK_ACTIONS) {
    settings.homeButtonDoubleTapAction = FluiDezSettings::HOME_BUTTON_TOGGLE_FRONTLIGHT;
  }
  if (settings.sideButtonUpShort == FluiDezSettings::QUICK_ACTIONS)
    settings.sideButtonUpShort = FluiDezSettings::IGNORE;
  if (settings.sideButtonUpLong == FluiDezSettings::QUICK_ACTIONS) settings.sideButtonUpLong = FluiDezSettings::IGNORE;
  if (settings.sideButtonDownShort == FluiDezSettings::QUICK_ACTIONS)
    settings.sideButtonDownShort = FluiDezSettings::IGNORE;
  if (settings.sideButtonDownLong == FluiDezSettings::QUICK_ACTIONS)
    settings.sideButtonDownLong = FluiDezSettings::IGNORE;

  if (trigger == Trigger::ShortPower) settings.shortPwrBtn = FluiDezSettings::QUICK_ACTIONS;
  if (trigger == Trigger::LongPower) settings.longPwrBtn = FluiDezSettings::QUICK_ACTIONS;
  if (trigger == Trigger::LongBack) settings.longPressBackAction = FluiDezSettings::LONG_MENU_QUICK_ACTIONS;
  if (trigger == Trigger::LongMenu) settings.longPressMenuAction = FluiDezSettings::LONG_MENU_QUICK_ACTIONS;
  if (trigger == Trigger::PowerUp) settings.powerChordAction = FluiDezSettings::CHORD_QUICK_ACTIONS;
  if (trigger == Trigger::UpDown) settings.sideButtonChordAction = FluiDezSettings::CHORD_QUICK_ACTIONS;
  if (trigger == Trigger::TapHome) settings.homeButtonTapAction = FluiDezSettings::QUICK_ACTIONS;
  if (trigger == Trigger::LongPressHome) settings.homeButtonLongPressAction = FluiDezSettings::QUICK_ACTIONS;
  if (trigger == Trigger::DoubleTapHome) settings.homeButtonDoubleTapAction = FluiDezSettings::QUICK_ACTIONS;
  if (trigger == Trigger::SideUpShort) settings.sideButtonUpShort = FluiDezSettings::QUICK_ACTIONS;
  if (trigger == Trigger::SideUpLong) settings.sideButtonUpLong = FluiDezSettings::QUICK_ACTIONS;
  if (trigger == Trigger::SideDownShort) settings.sideButtonDownShort = FluiDezSettings::QUICK_ACTIONS;
  if (trigger == Trigger::SideDownLong) settings.sideButtonDownLong = FluiDezSettings::QUICK_ACTIONS;
  settings.quickActionsTrigger = static_cast<uint8_t>(trigger);
}

inline Trigger triggerForSetting(uint8_t FluiDezSettings::* member) {
  if (member == &FluiDezSettings::shortPwrBtn) return Trigger::ShortPower;
  if (member == &FluiDezSettings::longPwrBtn) return Trigger::LongPower;
  if (member == &FluiDezSettings::longPressBackAction) return Trigger::LongBack;
  if (member == &FluiDezSettings::longPressMenuAction) return Trigger::LongMenu;
  if (member == &FluiDezSettings::powerChordAction) return Trigger::PowerUp;
  if (member == &FluiDezSettings::sideButtonChordAction) return Trigger::UpDown;
  if (member == &FluiDezSettings::homeButtonTapAction) return Trigger::TapHome;
  if (member == &FluiDezSettings::homeButtonLongPressAction) return Trigger::LongPressHome;
  if (member == &FluiDezSettings::homeButtonDoubleTapAction) return Trigger::DoubleTapHome;
  if (member == &FluiDezSettings::sideButtonUpShort) return Trigger::SideUpShort;
  if (member == &FluiDezSettings::sideButtonUpLong) return Trigger::SideUpLong;
  if (member == &FluiDezSettings::sideButtonDownShort) return Trigger::SideDownShort;
  if (member == &FluiDezSettings::sideButtonDownLong) return Trigger::SideDownLong;
  return Trigger::None;
}

inline void settingChanged(FluiDezSettings& settings, uint8_t FluiDezSettings::* member) {
  const Trigger trigger = triggerForSetting(member);
  if (trigger == Trigger::None) return;
  const bool selected = trigger == Trigger::LongBack || trigger == Trigger::LongMenu
                            ? settings.*member == FluiDezSettings::LONG_MENU_QUICK_ACTIONS
                        : (trigger == Trigger::PowerUp || trigger == Trigger::UpDown)
                            ? settings.*member == FluiDezSettings::CHORD_QUICK_ACTIONS
                            : settings.*member == FluiDezSettings::QUICK_ACTIONS;
  synchronize(settings, selected ? trigger : Trigger::None);
}

using ActionHandler = std::function<void(FluiDezSettings::SHORT_PWRBTN)>;
using ActionFilter = std::function<bool(FluiDezSettings::SHORT_PWRBTN)>;

void showConfiguredPopup(OptionPopup& popup, const std::function<void()>& requestUpdate,
                         ActionHandler actionHandler = {}, ActionFilter actionFilter = {});
}  // namespace QuickActions
