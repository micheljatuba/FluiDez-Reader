#include "SettingsList.h"

const std::vector<SettingInfo>& getBaseSettingsList() {
  static const std::vector<SettingInfo> baseList = [] {
    std::vector<SettingInfo> v;
    // Reserve the maximum final size. Growing this process-lifetime vector
    // would otherwise leave it holding roughly twice the memory it needs.
    v.reserve(BASE_SETTINGS_CAPACITY);
    auto add = [&v](SettingInfo setting) { v.push_back(std::move(setting)); };

    // --- Display ---
    add(buildSleepScreenSetting());
    add(SettingInfo::Enum(StrId::STR_SLEEP_COVER_MODE, &FluiDezSettings::sleepScreenCoverMode,
                          {StrId::STR_FIT, StrId::STR_CROP}, "sleepScreenCoverMode", StrId::STR_CAT_DISPLAY));
    add(SettingInfo::Enum(StrId::STR_SLEEP_COVER_FILTER, &FluiDezSettings::sleepScreenCoverFilter,
                          {StrId::STR_NONE_OPT, StrId::STR_FILTER_CONTRAST, StrId::STR_INVERTED},
                          "sleepScreenCoverFilter", StrId::STR_CAT_DISPLAY));
    add(SettingInfo::Toggle(StrId::STR_QUICK_RESUME_TIMEOUT, &FluiDezSettings::quickResumeSleepScreen,
                            "quickResumeSleepScreen", StrId::STR_CAT_DISPLAY));
    add(SettingInfo::Enum(StrId::STR_HIDE_BATTERY, &FluiDezSettings::hideBatteryPercentage,
                          {StrId::STR_NEVER, StrId::STR_IN_READER, StrId::STR_ALWAYS}, "hideBatteryPercentage",
                          StrId::STR_CAT_DISPLAY));
    add(SettingInfo::Enum(StrId::STR_HIDE_CLOCK, &FluiDezSettings::hideClock,
                          {StrId::STR_NEVER, StrId::STR_IN_READER, StrId::STR_ALWAYS}, "hideClock",
                          StrId::STR_CAT_DISPLAY)
            .withEnumRawValues({FluiDezSettings::HIDE_CLOCK_NEVER, FluiDezSettings::HIDE_CLOCK_IN_READER,
                                FluiDezSettings::HIDE_CLOCK_ALWAYS}));
    add(SettingInfo::Enum(StrId::STR_REFRESH_FREQ, &FluiDezSettings::refreshFrequency,
                          {StrId::STR_PAGES_1, StrId::STR_PAGES_5, StrId::STR_PAGES_10, StrId::STR_PAGES_15,
                           StrId::STR_PAGES_30, StrId::STR_NEVER},
                          "refreshFrequency", StrId::STR_CAT_DISPLAY)
            .withEnumRawValues({FluiDezSettings::REFRESH_1, FluiDezSettings::REFRESH_5, FluiDezSettings::REFRESH_10,
                                FluiDezSettings::REFRESH_15, FluiDezSettings::REFRESH_30,
                                FluiDezSettings::REFRESH_NEVER}));
    add(SettingInfo::Toggle(StrId::STR_NIGHT_MODE, &FluiDezSettings::screenInverted, "screenInverted",
                            StrId::STR_CAT_DISPLAY));
    add(SettingInfo::Enum(
            StrId::STR_UI_THEME, &FluiDezSettings::uiTheme,
            {StrId::STR_THEME_FLUIDEZ_SHELF, StrId::STR_THEME_FLUIDEZ_CARDS, StrId::STR_THEME_FLUIDEZ_FLUXO,
             StrId::STR_THEME_CLASSIC, StrId::STR_THEME_MINIMAL, StrId::STR_THEME_DASHBOARD, StrId::STR_THEME_LYRA,
             StrId::STR_THEME_LYRA_EXTENDED, StrId::STR_THEME_LYRA_CAROUSEL, StrId::STR_THEME_LYRA_GRID,
             StrId::STR_THEME_ROUNDEDRAFF, StrId::STR_THEME_COVER_GRID},
            "uiTheme", StrId::STR_CAT_DISPLAY)
            .withEnumRawValues({FluiDezSettings::UI_THEME::FLUIDEZ_SHELF, FluiDezSettings::UI_THEME::FLUIDEZ_CARDS,
                                FluiDezSettings::UI_THEME::FLUIDEZ_FLUXO, FluiDezSettings::UI_THEME::CLASSIC,
                                FluiDezSettings::UI_THEME::MINIMAL, FluiDezSettings::UI_THEME::DASHBOARD,
                                FluiDezSettings::UI_THEME::LYRA, FluiDezSettings::UI_THEME::LYRA_3_COVERS,
                                FluiDezSettings::UI_THEME::LYRA_CAROUSEL, FluiDezSettings::UI_THEME::LYRA_GRID,
                                FluiDezSettings::UI_THEME::ROUNDEDRAFF, FluiDezSettings::UI_THEME::COVER_GRID}));
    add(SettingInfo::Toggle(StrId::STR_SWAP_LIBRARY_FILE_BROWSER, &FluiDezSettings::swapLibraryFileBrowser,
                            "swapLibraryFileBrowser", StrId::STR_CAT_DISPLAY));
    add(SettingInfo::Enum(StrId::STR_UI_SCALE, &FluiDezSettings::uiScale, {StrId::STR_SMALL, StrId::STR_LARGE},
                          "uiScale", StrId::STR_CAT_DISPLAY)
            .withEnumRawValues({FluiDezSettings::UI_SCALE_SMALL, FluiDezSettings::UI_SCALE_LARGE}));
    add(SettingInfo::Toggle(StrId::STR_LIBRARY_USE_METADATA, &FluiDezSettings::libraryUseMetadata, "libraryUseMetadata",
                            StrId::STR_CAT_DISPLAY));
    add(SettingInfo::Toggle(StrId::STR_SUNLIGHT_FADING_FIX, &FluiDezSettings::fadingFix, "fadingFix",
                            StrId::STR_CAT_DISPLAY));
    add(SettingInfo::Toggle(StrId::STR_RESTORE_LIGHT_ON_WAKE, &FluiDezSettings::frontlightRestoreOnWake,
                            "frontlightRestoreOnWake", StrId::STR_CAT_DISPLAY));
    // Kept in the shared catalog for persistence and the web API. On-device,
    // these values are presented only by Display > Frontlight.
    add(SettingInfo::Toggle(StrId::STR_FRONTLIGHT_SCHEDULE, &FluiDezSettings::frontlightScheduleEnabled,
                            "frontlightScheduleEnabled", StrId::STR_CAT_DISPLAY));
    add(SettingInfo::Value16(StrId::STR_START, &FluiDezSettings::frontlightScheduleStart,
                             {0, FrontlightSchedule::kUnsetTimeOfDay, 1}, "frontlightScheduleStart",
                             StrId::STR_CAT_DISPLAY));
    add(SettingInfo::Value16(StrId::STR_END, &FluiDezSettings::frontlightScheduleEnd,
                             {0, FrontlightSchedule::kUnsetTimeOfDay, 1}, "frontlightScheduleEnd",
                             StrId::STR_CAT_DISPLAY));

    // --- Reader ---
    // Built-in font-family entry. Replaced per-call with a registry-aware
    // version when SD fonts are installed.
    add(SettingInfo::Enum(StrId::STR_FONT_FAMILY, &FluiDezSettings::fontFamily,
                          {StrId::STR_LEXEND_DECA, StrId::STR_BITTER}, "fontFamily", StrId::STR_CAT_READER));
    add(buildBuiltinFontSizeSetting());
    add(SettingInfo::Enum(StrId::STR_SD_FONT_SIZE_RANGE, &FluiDezSettings::sdFontSizeRange,
                          {StrId::STR_FONT_RANGE_TEENSY, StrId::STR_FONT_RANGE_TINY, StrId::STR_FONT_RANGE_XLARGE,
                           StrId::STR_FONT_RANGE_ALL},
                          "sdFontSizeRange", StrId::STR_CAT_READER)
            .withEnumRawValues({FluiDezSettings::SD_FONT_RANGE_TEENSY, FluiDezSettings::SD_FONT_RANGE_TINY,
                                FluiDezSettings::SD_FONT_RANGE_XLARGE, FluiDezSettings::SD_FONT_RANGE_ALL}));
    add(SettingInfo::Value(StrId::STR_LINE_SPACING, &FluiDezSettings::lineHeightPercent,
                           {FluiDezSettings::MIN_LINE_HEIGHT_PERCENT, FluiDezSettings::MAX_LINE_HEIGHT_PERCENT,
                            FluiDezSettings::LINE_HEIGHT_PERCENT_STEP},
                           "lineHeightPercent", StrId::STR_CAT_READER));
    add(SettingInfo::Value(StrId::STR_WORD_SPACING, &FluiDezSettings::wordSpacing,
                           {0, FluiDezSettings::MAX_WORD_SPACING, 1}, "wordSpacing", StrId::STR_CAT_READER));
    add(SettingInfo::Enum(
            StrId::STR_ORIENTATION, &FluiDezSettings::orientation,
            {StrId::STR_PORTRAIT, StrId::STR_LANDSCAPE_CW, StrId::STR_LANDSCAPE_CCW, StrId::STR_ORIENTATION_INVERTED},
            "orientation", StrId::STR_CAT_READER)
            .withEnumRawValues({FluiDezSettings::PORTRAIT, FluiDezSettings::LANDSCAPE_CW,
                                FluiDezSettings::LANDSCAPE_CCW, FluiDezSettings::INVERTED}));
    add(SettingInfo::Submenu(StrId::STR_SCREEN_MARGIN, SettingAction::ScreenMargin));
    add(SettingInfo::Value(StrId::STR_TOP_BOTTOM, &FluiDezSettings::screenMarginVertical,
                           {FluiDezSettings::MIN_SCREEN_MARGIN, FluiDezSettings::MAX_SCREEN_MARGIN,
                            FluiDezSettings::SCREEN_MARGIN_SMALL_STEP},
                           "screenMarginVertical", StrId::STR_CAT_READER));
    add(SettingInfo::Value(StrId::STR_LEFT_RIGHT, &FluiDezSettings::screenMarginHorizontal,
                           {FluiDezSettings::MIN_SCREEN_MARGIN, FluiDezSettings::MAX_SCREEN_MARGIN,
                            FluiDezSettings::SCREEN_MARGIN_SMALL_STEP},
                           "screenMarginHorizontal", StrId::STR_CAT_READER));
    add(SettingInfo::Toggle(StrId::STR_PUBLISHER_PAGE_NUMBERS, &FluiDezSettings::publisherPageNumbers,
                            "publisherPageNumbers", StrId::STR_CAT_READER));
    add(SettingInfo::Enum(
        StrId::STR_PARA_ALIGNMENT, &FluiDezSettings::paragraphAlignment,
        {StrId::STR_JUSTIFY, StrId::STR_ALIGN_LEFT, StrId::STR_CENTER, StrId::STR_ALIGN_RIGHT, StrId::STR_BOOK_S_STYLE},
        "paragraphAlignment", StrId::STR_CAT_READER));
    add(SettingInfo::Toggle(StrId::STR_EMBEDDED_STYLE, &FluiDezSettings::embeddedStyle, "embeddedStyle",
                            StrId::STR_CAT_READER));
    add(SettingInfo::Toggle(StrId::STR_HYPHENATION, &FluiDezSettings::hyphenationEnabled, "hyphenationEnabled",
                            StrId::STR_CAT_READER));
    add(SettingInfo::Toggle(StrId::STR_TEXT_AA, &FluiDezSettings::textAntiAliasing, "textAntiAliasing",
                            StrId::STR_CAT_READER));
    add(SettingInfo::Enum(StrId::STR_IMAGES, &FluiDezSettings::imageRendering,
                          {StrId::STR_IMAGES_DISPLAY, StrId::STR_IMAGES_PLACEHOLDER, StrId::STR_IMAGES_SUPPRESS},
                          "imageRendering", StrId::STR_CAT_READER));
    add(SettingInfo::Toggle(StrId::STR_TOUCH_READER_CONTROLS, &FluiDezSettings::touchReaderControls,
                            "touchReaderControls", StrId::STR_CAT_READER));
    add(SettingInfo::Toggle(StrId::STR_DISABLE_TOUCHSCREEN, &FluiDezSettings::disableReaderTouchscreen,
                            "disableReaderTouchscreen", StrId::STR_CAT_READER));
    add(SettingInfo::Toggle(StrId::STR_EXTRA_SPACING, &FluiDezSettings::extraParagraphSpacing, "extraParagraphSpacing",
                            StrId::STR_CAT_READER));
    add(SettingInfo::Toggle(StrId::STR_FORCE_PARAGRAPH_INDENTS, &FluiDezSettings::forceParagraphIndents,
                            "forceParagraphIndents", StrId::STR_CAT_READER));
    add(SettingInfo::Toggle(StrId::STR_FOCUS_READING, &FluiDezSettings::focusReadingEnabled, "focusReadingEnabled",
                            StrId::STR_CAT_READER));
    add(SettingInfo::Toggle(StrId::STR_GUIDE_READING, &FluiDezSettings::guideReadingEnabled, "guideReadingEnabled",
                            StrId::STR_CAT_READER));
    add(SettingInfo::Enum(StrId::STR_INDEXING_METHOD, &FluiDezSettings::indexingMethod,
                          {StrId::STR_INDEXING_INCREMENTAL, StrId::STR_INDEXING_FULL_SECTION}, "indexingMethod",
                          StrId::STR_CAT_READER));

    // --- Controls ---
    add(SettingInfo::Toggle(StrId::STR_PINCH_FONT_RESIZE, &FluiDezSettings::pinchFontResizeEnabled,
                            "pinchFontResizeEnabled", StrId::STR_CAT_CONTROLS));
    add(SettingInfo::Toggle(StrId::STR_TWO_FINGER_ROTATION, &FluiDezSettings::twoFingerRotationEnabled,
                            "twoFingerRotationEnabled", StrId::STR_CAT_CONTROLS));
    const std::vector<StrId> twoFingerSwipeActions = {
        StrId::STR_NOT_SET,          StrId::STR_INCREASE_BRIGHTNESS, StrId::STR_DECREASE_BRIGHTNESS,
        StrId::STR_INCREASE_WARMTH,  StrId::STR_DECREASE_WARMTH,     StrId::STR_NEXT_CHAPTER,
        StrId::STR_PREVIOUS_CHAPTER, StrId::STR_INCREASE_FONT_SIZE,  StrId::STR_DECREASE_FONT_SIZE,
    };
    const std::vector<uint8_t> twoFingerSwipeActionValues = {
        FluiDezSettings::TWO_FINGER_SWIPE_NOT_SET,
        FluiDezSettings::TWO_FINGER_SWIPE_INCREASE_BRIGHTNESS,
        FluiDezSettings::TWO_FINGER_SWIPE_DECREASE_BRIGHTNESS,
        FluiDezSettings::TWO_FINGER_SWIPE_INCREASE_WARMTH,
        FluiDezSettings::TWO_FINGER_SWIPE_DECREASE_WARMTH,
        FluiDezSettings::TWO_FINGER_SWIPE_NEXT_CHAPTER,
        FluiDezSettings::TWO_FINGER_SWIPE_PREVIOUS_CHAPTER,
        FluiDezSettings::TWO_FINGER_SWIPE_INCREASE_FONT_SIZE,
        FluiDezSettings::TWO_FINGER_SWIPE_DECREASE_FONT_SIZE,
    };
    add(SettingInfo::Enum(StrId::STR_TWO_FINGER_SWIPE_UP, &FluiDezSettings::twoFingerSwipeUp, twoFingerSwipeActions,
                          "twoFingerSwipeUp", StrId::STR_CAT_CONTROLS)
            .withEnumRawValues(twoFingerSwipeActionValues));
    add(SettingInfo::Enum(StrId::STR_TWO_FINGER_SWIPE_DOWN, &FluiDezSettings::twoFingerSwipeDown, twoFingerSwipeActions,
                          "twoFingerSwipeDown", StrId::STR_CAT_CONTROLS)
            .withEnumRawValues(twoFingerSwipeActionValues));
    add(SettingInfo::Enum(StrId::STR_TWO_FINGER_SWIPE_LEFT, &FluiDezSettings::twoFingerSwipeLeft, twoFingerSwipeActions,
                          "twoFingerSwipeLeft", StrId::STR_CAT_CONTROLS)
            .withEnumRawValues(twoFingerSwipeActionValues));
    add(SettingInfo::Enum(StrId::STR_TWO_FINGER_SWIPE_RIGHT, &FluiDezSettings::twoFingerSwipeRight,
                          twoFingerSwipeActions, "twoFingerSwipeRight", StrId::STR_CAT_CONTROLS)
            .withEnumRawValues(twoFingerSwipeActionValues));
#if defined(FLUIDEZ_APP_CAP_TOUCH) && FLUIDEZ_APP_CAP_TOUCH
    add(SettingInfo::Enum(StrId::STR_LEFT_EDGE_UP, &FluiDezSettings::leftEdgeUp, twoFingerSwipeActions, "leftEdgeUp",
                          StrId::STR_CAT_CONTROLS)
            .withEnumRawValues(twoFingerSwipeActionValues));
    add(SettingInfo::Enum(StrId::STR_LEFT_EDGE_DOWN, &FluiDezSettings::leftEdgeDown, twoFingerSwipeActions,
                          "leftEdgeDown", StrId::STR_CAT_CONTROLS)
            .withEnumRawValues(twoFingerSwipeActionValues));
    add(SettingInfo::Enum(StrId::STR_RIGHT_EDGE_UP, &FluiDezSettings::rightEdgeUp, twoFingerSwipeActions, "rightEdgeUp",
                          StrId::STR_CAT_CONTROLS)
            .withEnumRawValues(twoFingerSwipeActionValues));
    add(SettingInfo::Enum(StrId::STR_RIGHT_EDGE_DOWN, &FluiDezSettings::rightEdgeDown, twoFingerSwipeActions,
                          "rightEdgeDown", StrId::STR_CAT_CONTROLS)
            .withEnumRawValues(twoFingerSwipeActionValues));
#endif
    add(SettingInfo::Toggle(StrId::STR_ORIENTATION_AWARE, &FluiDezSettings::sideButtonOrientationAware,
                            "sideButtonOrientationAware", StrId::STR_CAT_CONTROLS));
    add(buildSideButtonActionSetting(StrId::STR_SHORT_PWR_BTN, &FluiDezSettings::sideButtonUpShort,
                                     "sideButtonUpShort"));
    add(buildSideButtonActionSetting(StrId::STR_LONG_PRESS_ACTION, &FluiDezSettings::sideButtonUpLong,
                                     "sideButtonUpLong"));
    add(buildSideButtonActionSetting(StrId::STR_SHORT_PWR_BTN, &FluiDezSettings::sideButtonDownShort,
                                     "sideButtonDownShort"));
    add(buildSideButtonActionSetting(StrId::STR_LONG_PRESS_ACTION, &FluiDezSettings::sideButtonDownLong,
                                     "sideButtonDownLong"));
    add(SettingInfo::Enum(StrId::STR_ORIENTATION_AWARE, &FluiDezSettings::frontButtonOrientationAware,
                          {StrId::STR_NO, StrId::STR_NAV_BUTTONS, StrId::STR_ALL_BUTTONS},
                          "frontButtonOrientationAware", StrId::STR_CAT_CONTROLS));
    add(SettingInfo::Enum(StrId::STR_LONG_PRESS_ACTION, &FluiDezSettings::longPressButtonBehavior,
                          {StrId::STR_LONG_PRESS_BEHAVIOR_OFF, StrId::STR_LONG_PRESS_BEHAVIOR_SKIP,
                           StrId::STR_CHANGE_FONT_SIZE, StrId::STR_LONG_PRESS_BEHAVIOR_ORIENTATION},
                          "longPressButtonBehavior", StrId::STR_CAT_CONTROLS)
            .withEnumRawValues({FluiDezSettings::OFF, FluiDezSettings::CHAPTER_SKIP, FluiDezSettings::FONT_SIZE_CHANGE,
                                FluiDezSettings::ORIENTATION_CHANGE}));
    add(buildShortcutSetting(StrId::STR_SHORT_PWR_BTN, &FluiDezSettings::shortPwrBtn, "shortPwrBtn",
                             ShortcutOptionCatalog::PowerButton));
    add(buildShortcutSetting(StrId::STR_LONG_PRESS_ACTION, &FluiDezSettings::longPwrBtn, "longPwrBtn",
                             ShortcutOptionCatalog::PowerButton));
    add(buildShortcutSetting(StrId::STR_POWER_BUTTON_CHORD, &FluiDezSettings::powerChordAction, "powerChordAction",
                             ShortcutOptionCatalog::ButtonChord));
    add(buildShortcutSetting(StrId::STR_SIDE_BUTTON_CHORD, &FluiDezSettings::sideButtonChordAction,
                             "sideButtonChordAction", ShortcutOptionCatalog::ButtonChord));
    add(SettingInfo::Enum(StrId::STR_IN_READER, &FluiDezSettings::homeButtonInReaderEnabled,
                          {StrId::STR_ENABLED, StrId::STR_DISABLED}, "homeButtonInReaderEnabled",
                          StrId::STR_CAT_CONTROLS)
            .withEnumRawValues({1, 0}));
    add(buildHomeButtonActionSetting(StrId::STR_HOME_BUTTON_TAP, &FluiDezSettings::homeButtonTapAction,
                                     "homeButtonTapAction"));
    add(buildHomeButtonActionSetting(StrId::STR_HOME_BUTTON_DOUBLE_TAP, &FluiDezSettings::homeButtonDoubleTapAction,
                                     "homeButtonDoubleTapAction"));
    add(buildHomeButtonActionSetting(StrId::STR_LONG_PRESS_ACTION, &FluiDezSettings::homeButtonLongPressAction,
                                     "homeButtonLongPressAction"));
    add(buildShortcutSetting(StrId::STR_LONG_PRESS_MENU_ACTION, &FluiDezSettings::longPressMenuAction,
                             "longPressMenuAction", ShortcutOptionCatalog::LongPress));
    add(buildShortcutSetting(StrId::STR_LONG_PRESS_BACK_ACTION, &FluiDezSettings::longPressBackAction,
                             "longPressBackAction", ShortcutOptionCatalog::LongPress));
    add(SettingInfo::Toggle(StrId::STR_PWR_BTN_FOOTNOTE_BACK, &FluiDezSettings::pwrBtnFootnoteBack,
                            "pwrBtnFootnoteBack", StrId::STR_CAT_CONTROLS));
    add(SettingInfo::Enum(StrId::STR_NEXT_PAGE, &FluiDezSettings::pageTurnGesture,
                          {StrId::STR_TAP_AND_SWIPE, StrId::STR_TAP_ONLY, StrId::STR_SWIPE_ONLY,
                           StrId::STR_INVERTED_TAP, StrId::STR_DISABLED},
                          "pageTurnGesture", StrId::STR_CAT_CONTROLS));
    add(SettingInfo::Enum(StrId::STR_PREV_PAGE, &FluiDezSettings::previousPageGesture,
                          {StrId::STR_TAP_AND_SWIPE, StrId::STR_TAP_ONLY, StrId::STR_SWIPE_ONLY,
                           StrId::STR_INVERTED_TAP, StrId::STR_DISABLED},
                          "previousPageGesture", StrId::STR_CAT_CONTROLS));
    add(SettingInfo::Toggle(StrId::STR_TAP_HIDE_STATUS_BAR, &FluiDezSettings::tapToHideStatusBar, "tapToHideStatusBar",
                            StrId::STR_CAT_CONTROLS));

    // --- System ---
    add(SettingInfo::String(StrId::STR_DEVICE_NAME, SETTINGS.deviceName, sizeof(SETTINGS.deviceName), "deviceName",
                            StrId::STR_CAT_SYSTEM));
    add(SettingInfo::Value(StrId::STR_TIME_TO_SLEEP, &FluiDezSettings::sleepTimeoutMinutes,
                           {FluiDezSettings::MIN_SLEEP_TIMEOUT_MINUTES, FluiDezSettings::MAX_SLEEP_TIMEOUT_MINUTES, 1},
                           "sleepTimeoutMinutes", StrId::STR_CAT_SYSTEM));
    add(SettingInfo::Toggle(StrId::STR_CUSTOM_BOOTSCREEN, &FluiDezSettings::customBootscreenEnabled,
                            "customBootscreenEnabled", StrId::STR_CAT_SYSTEM));
    add(SettingInfo::Toggle(StrId::STR_SHOW_HIDDEN_FILES, &FluiDezSettings::showHiddenFiles, "showHiddenFiles",
                            StrId::STR_CAT_SYSTEM));
    add(SettingInfo::Toggle(StrId::STR_HIDE_FILE_EXTENSION, &FluiDezSettings::hideFileExtension, "hideFileExtension",
                            StrId::STR_CAT_SYSTEM));
    add(SettingInfo::Enum(StrId::STR_FILE_BROWSER_DISPLAY, &FluiDezSettings::fileBrowserDisplay,
                          {StrId::STR_FILE_BROWSER_DISPLAY_1_LINE, StrId::STR_FILE_BROWSER_DISPLAY_2_LINES},
                          "fileBrowserDisplay", StrId::STR_CAT_SYSTEM));
    add(SettingInfo::Toggle(StrId::STR_REMOVE_READ_FROM_RECENTS, &FluiDezSettings::removeReadBooksFromRecents,
                            "removeReadBooksFromRecents", StrId::STR_CAT_SYSTEM));
    add(SettingInfo::Toggle(StrId::STR_MOVE_FINISHED_TO_READ, &FluiDezSettings::moveFinishedToReadFolder,
                            "moveFinishedToReadFolder", StrId::STR_CAT_SYSTEM));
    add(SettingInfo::Toggle(StrId::STR_AUTO_BACKUP_STATS, &FluiDezSettings::autoBackupStats, "autoBackupStats",
                            StrId::STR_CAT_SYSTEM));
    // Persisted and available to the web settings API, but category-less because
    // the on-device editor lives under System > OPDS Servers.
    add(SettingInfo::String(StrId::STR_OPDS_DOWNLOAD_FOLDER, SETTINGS.opdsDownloadFolder,
                            sizeof(SETTINGS.opdsDownloadFolder), "opdsDownloadFolder"));
    // Persisted here, but edited from the nearby receive screen's folder picker.
    add(SettingInfo::String(StrId::STR_NEARBY_RECEIVE_FOLDER, SETTINGS.nearbyReceiveFolder,
                            sizeof(SETTINGS.nearbyReceiveFolder), "nearbyReceiveFolder"));
    add(SettingInfo::Value(StrId::STR_IDLE_TIME_THRESHOLD, &FluiDezSettings::readingIdleTimeThresholdUnits,
                           {FluiDezSettings::MIN_READING_IDLE_TIME_THRESHOLD_UNITS,
                            FluiDezSettings::MAX_READING_IDLE_TIME_THRESHOLD_UNITS, 1},
                           "readingIdleTimeThresholdUnits", StrId::STR_CAT_SYSTEM));
    add(SettingInfo::Toggle(StrId::STR_TRACK_READING_STATS, &FluiDezSettings::trackReadingStats, "trackReadingStats",
                            StrId::STR_CAT_SYSTEM));

    // Frontlight quick-panel state: persisted + web-exposed, category-less so
    // it stays off the Settings screen (edited from the swipe-down panel).
    add(SettingInfo::Value(StrId::STR_BRIGHTNESS, &FluiDezSettings::frontlightBrightness, {0, 100, 5},
                           "frontlightBrightness"));
    add(SettingInfo::Value(StrId::STR_WARMTH, &FluiDezSettings::frontlightWarmth, {0, 100, 5}, "frontlightWarmth"));
    add(SettingInfo::Toggle(StrId::STR_FRONTLIGHT, &FluiDezSettings::frontlightOn, "frontlightOn"));

    // --- KOReader Sync (web-only, uses KOReaderCredentialStore) ---
    add(SettingInfo::DynamicString(
        StrId::STR_KOREADER_USERNAME, [] { return KOREADER_STORE.getUsername(); },
        [](const std::string& v) {
          KOREADER_STORE.setCredentials(v, KOREADER_STORE.getPassword());
          KOREADER_STORE.saveToFile();
        },
        "koUsername", StrId::STR_KOREADER_SYNC));
    add(SettingInfo::DynamicString(
        StrId::STR_KOREADER_PASSWORD, [] { return KOREADER_STORE.getPassword(); },
        [](const std::string& v) {
          KOREADER_STORE.setCredentials(KOREADER_STORE.getUsername(), v);
          KOREADER_STORE.saveToFile();
        },
        "koPassword", StrId::STR_KOREADER_SYNC));
    add(SettingInfo::DynamicString(
        StrId::STR_SYNC_SERVER_URL, [] { return KOREADER_STORE.getServerUrl(); },
        [](const std::string& v) {
          KOREADER_STORE.setServerUrl(v);
          KOREADER_STORE.saveToFile();
        },
        "koServerUrl", StrId::STR_KOREADER_SYNC));
    add(SettingInfo::DynamicEnum(
        StrId::STR_DOCUMENT_MATCHING, {StrId::STR_FILENAME, StrId::STR_BINARY},
        [] { return static_cast<uint8_t>(KOREADER_STORE.getMatchMethod()); },
        [](uint8_t v) {
          KOREADER_STORE.setMatchMethod(static_cast<DocumentMatchMethod>(v));
          KOREADER_STORE.saveToFile();
        },
        "koMatchMethod", StrId::STR_KOREADER_SYNC));
    add(SettingInfo::DynamicEnum(
        StrId::STR_SEND_METADATA, {StrId::STR_STATE_OFF, StrId::STR_STATE_ON},
        [] { return static_cast<uint8_t>(KOREADER_STORE.getSendMetadata()); },
        [](uint8_t v) {
          KOREADER_STORE.setSendMetadata(v != 0);
          KOREADER_STORE.saveToFile();
        },
        "koSendMetadata", StrId::STR_KOREADER_SYNC));

    add(SettingInfo::DynamicEnum(
        StrId::STR_SYNC_BEHAVIOR, {StrId::STR_ASK_EVERY_TIME, StrId::STR_SMART_SYNC},
        [] { return static_cast<uint8_t>(KOREADER_STORE.getSyncBehavior()); },
        [](uint8_t v) {
          KOREADER_STORE.setSyncBehavior(static_cast<KOReaderSyncBehavior>(v));
          KOREADER_STORE.saveToFile();
        },
        "koSyncBehavior", StrId::STR_KOREADER_SYNC));

    // Legacy fields stay in JSON for one-time status bar migration; the web
    // editor uses /api/status-bars instead of exposing these controls.
    add(SettingInfo::Toggle(StrId::STR_CHAPTER_PAGE_COUNT, &FluiDezSettings::statusBarChapterPageCount,
                            "statusBarChapterPageCount", StrId::STR_STATUS_BARS));
    add(SettingInfo::Toggle(StrId::STR_STABLE_PAGE_NUMBERS, &FluiDezSettings::stablePageNumbers, "stablePageNumbers",
                            StrId::STR_STATUS_BARS));
    add(SettingInfo::Toggle(StrId::STR_BOOK_PROGRESS_PERCENTAGE, &FluiDezSettings::statusBarBookProgressPercentage,
                            "statusBarBookProgressPercentage", StrId::STR_STATUS_BARS));
    add(SettingInfo::Enum(StrId::STR_PERCENTAGE_FORMAT, &FluiDezSettings::statusBarBookPercentageFormat,
                          {StrId::STR_PERCENTAGE_FORMAT_WHOLE, StrId::STR_PERCENTAGE_FORMAT_ONE_DECIMAL,
                           StrId::STR_PERCENTAGE_FORMAT_TWO_DECIMALS},
                          "statusBarBookPercentageFormat", StrId::STR_STATUS_BARS));
    add(SettingInfo::Enum(StrId::STR_PROGRESS_BAR, &FluiDezSettings::statusBarProgressBar,
                          {StrId::STR_HIDE, StrId::STR_BOOK, StrId::STR_CHAPTER}, "statusBarProgressBar",
                          StrId::STR_STATUS_BARS)
            .withEnumRawValues(
                {FluiDezSettings::HIDE_PROGRESS, FluiDezSettings::BOOK_PROGRESS, FluiDezSettings::CHAPTER_PROGRESS}));
    add(SettingInfo::Enum(StrId::STR_PROGRESS_BAR_THICKNESS, &FluiDezSettings::statusBarProgressBarThickness,
                          {StrId::STR_PROGRESS_BAR_THIN, StrId::STR_PROGRESS_BAR_MEDIUM, StrId::STR_PROGRESS_BAR_THICK},
                          "statusBarProgressBarThickness", StrId::STR_STATUS_BARS));
    add(SettingInfo::Enum(StrId::STR_TITLE, &FluiDezSettings::statusBarTitle,
                          {StrId::STR_HIDE, StrId::STR_BOOK, StrId::STR_CHAPTER}, "statusBarTitle",
                          StrId::STR_STATUS_BARS)
            .withEnumRawValues(
                {FluiDezSettings::HIDE_TITLE, FluiDezSettings::BOOK_TITLE, FluiDezSettings::CHAPTER_TITLE}));
    add(SettingInfo::Enum(StrId::STR_TIME_LEFT, &FluiDezSettings::statusBarTimeLeft,
                          {StrId::STR_HIDE, StrId::STR_CHAPTER, StrId::STR_BOOK}, "statusBarTimeLeft",
                          StrId::STR_STATUS_BARS));
    add(SettingInfo::Toggle(StrId::STR_BATTERY, &FluiDezSettings::statusBarBattery, "statusBarBattery",
                            StrId::STR_STATUS_BARS));
    add(SettingInfo::Enum(StrId::STR_XTC_STATUS_BAR, &FluiDezSettings::xtcStatusBarMode,
                          {StrId::STR_HIDE, StrId::STR_BOTTOM, StrId::STR_TOP, StrId::STR_STATUS_BAR_BOTH},
                          "xtcStatusBarMode", StrId::STR_STATUS_BARS));
    // Clock detail entries live under System > Device in the device UI.
    // Range 0..104 = quarter-hour steps from UTC-12:00 to UTC+14:00, biased by 48.
    add(SettingInfo::Value(StrId::STR_CLOCK_UTC_OFFSET, &FluiDezSettings::clockUtcOffsetQ, {0, 104, 1},
                           "clockUtcOffsetQ", StrId::STR_CAT_SYSTEM));
    add(SettingInfo::Enum(StrId::STR_CLOCK_FORMAT, &FluiDezSettings::clockFormat,
                          {StrId::STR_CLOCK_FORMAT_24H, StrId::STR_CLOCK_FORMAT_12H}, "clockFormat",
                          StrId::STR_CAT_SYSTEM));
    add(SettingInfo::Enum(StrId::STR_DATE_FORMAT, &FluiDezSettings::dateFormat,
                          {StrId::STR_DATE_FORMAT_MONTH_DAY_YEAR_LONG, StrId::STR_DATE_FORMAT_DAY_MONTH_YEAR_LONG,
                           StrId::STR_DATE_FORMAT_MONTH_DAY_YEAR_NUMERIC, StrId::STR_DATE_FORMAT_DAY_MONTH_YEAR_NUMERIC,
                           StrId::STR_DATE_FORMAT_YEAR_MONTH_DAY_NUMERIC, StrId::STR_DATE_FORMAT_MONTH_DAY_NUMERIC,
                           StrId::STR_DATE_FORMAT_DAY_MONTH_NUMERIC, StrId::STR_DATE_FORMAT_MONTH_DAY_LONG,
                           StrId::STR_DATE_FORMAT_DAY_MONTH_LONG},
                          "dateFormat", StrId::STR_CAT_SYSTEM));
    add(SettingInfo::Enum(
        StrId::STR_DATE_SEPARATOR, &FluiDezSettings::dateSeparator,
        {StrId::STR_DATE_SEPARATOR_PERIOD, StrId::STR_DATE_SEPARATOR_HYPHEN, StrId::STR_DATE_SEPARATOR_SLASH},
        "dateSeparator", StrId::STR_CAT_SYSTEM));
    // Persistence flag for NTP debounce. Resetting from the web UI forces a re-sync
    // on next WiFi connect, which is useful when crossing time zones.
    add(SettingInfo::Toggle(StrId::STR_CLOCK_SYNCED, &FluiDezSettings::clockHasBeenSynced, "clockHasBeenSynced",
                            StrId::STR_CAT_SYSTEM));
    // Only show tilt page turn settings when the active device has a supported IMU.
    if (QuickActions::supportsTiltPageTurn()) {
      auto shortPowerButtonIt = std::find_if(
          v.begin(), v.end(), [](const SettingInfo& setting) { return settingKeyIs(setting, "shortPwrBtn"); });
      if (shortPowerButtonIt != v.end()) {
        auto insertPos = v.insert(shortPowerButtonIt + 1,
                                  SettingInfo::Toggle(StrId::STR_TILT_PAGE_TURN, &FluiDezSettings::tiltPageTurn,
                                                      "tiltPageTurn", StrId::STR_CAT_CONTROLS));
        v.insert(
            insertPos + 1,
            SettingInfo::Enum(StrId::STR_TILT_PAGE_TURN_DIRECTION, &FluiDezSettings::tiltPageTurnDirection,
#if FLUIDEZ_APP_DEVICE_X4CLASSIC || defined(SIMULATOR_DEVICE_X4_CLASSIC)
                              // X4 Classic's X-axis has the opposite sign from the original X3 calibration.
                              // Keep the stored direction, but name its physical motion accurately.
                              {StrId::STR_TILT_DIRECTION_LEFT_RIGHT_INVERTED, StrId::STR_TILT_DIRECTION_LEFT_RIGHT,
                               StrId::STR_TILT_DIRECTION_FORWARD_BACK, StrId::STR_TILT_DIRECTION_FORWARD_BACK_INVERTED},
#else
                              {StrId::STR_TILT_DIRECTION_LEFT_RIGHT, StrId::STR_TILT_DIRECTION_LEFT_RIGHT_INVERTED,
                               StrId::STR_TILT_DIRECTION_FORWARD_BACK, StrId::STR_TILT_DIRECTION_FORWARD_BACK_INVERTED},
#endif
                              "tiltPageTurnDirection", StrId::STR_CAT_CONTROLS));
      }
    } else {
      for (auto& setting : v) {
        if (settingKeyIs(setting, "shortPwrBtn") || settingKeyIs(setting, "longPwrBtn")) {
          removeEnumRawValue(setting, static_cast<uint8_t>(FluiDezSettings::TOGGLE_TILT_PAGE_TURN));
        } else if (setting.nameId == StrId::STR_LONG_PRESS_MENU_ACTION ||
                   setting.nameId == StrId::STR_LONG_PRESS_BACK_ACTION) {
          removeEnumRawValue(setting, static_cast<uint8_t>(FluiDezSettings::LONG_MENU_TOGGLE_TILT_PAGE_TURN));
        }
      }
    }

    if (!gpio.deviceIsX3()) {
      auto sleepScreenIt =
          std::find_if(v.begin(), v.end(), [](const SettingInfo& s) { return s.nameId == StrId::STR_SLEEP_SCREEN; });
      if (sleepScreenIt != v.end()) {
        removeEnumRawValue(*sleepScreenIt, static_cast<uint8_t>(FluiDezSettings::MINIMAL_STATS_SLEEP));
      }
    }
    return v;
  }();

  return baseList;
}
