#include <AppVersion.h>

// Only this translation unit receives the changing version and Git defines.
#ifndef FLUIDEZ_VERSION
#define FLUIDEZ_VERSION "dev"
#endif

#ifndef FLUIDEZ_GIT_SHA
#define FLUIDEZ_GIT_SHA "unknown"
#endif

#ifndef FLUIDEZ_GIT_DIRTY
#define FLUIDEZ_GIT_DIRTY "unknown"
#endif

namespace AppVersion {
const char* version() { return FLUIDEZ_VERSION; }
const char* versionLabel() { return "FluiDez Reader " FLUIDEZ_VERSION; }
const char* userAgent() { return "FluiDez-Reader-ESP32-" FLUIDEZ_VERSION; }
const char* gitSha() { return FLUIDEZ_GIT_SHA; }
const char* gitDirtyFlag() { return FLUIDEZ_GIT_DIRTY; }
}  // namespace AppVersion
