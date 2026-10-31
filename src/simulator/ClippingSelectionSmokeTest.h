#pragma once
#if defined(SIMULATOR) && FLUIDEZ_APP_CAP_TOUCH
class EpubReaderActivity;
struct ClippingSelectionSmokeTest {
  static bool tick();
  static void verifySavedHighlightSpacing(EpubReaderActivity& reader);
};
#endif
