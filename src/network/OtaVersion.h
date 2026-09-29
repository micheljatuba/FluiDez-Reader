#pragma once

#include <cstddef>

// Orders an OTA release tag against the running CROSSINK_VERSION. Numeric
// segments decide first ("v1.7-fluidez1" > "1.6-fluidez9"). FluiDez builds keep
// the upstream base version and count their own releases after "fluidez"
// ("1.6-fluidez8-x4-pro"), so that build number breaks numeric ties.
namespace OtaVersion {
constexpr size_t SEGMENT_COUNT = 4;
constexpr int NO_FLUIDEZ_BUILD = -1;

struct Parsed {
  int segments[SEGMENT_COUNT] = {0, 0, 0, 0};
  int fluidezBuild = NO_FLUIDEZ_BUILD;
  bool valid = false;
  bool releaseCandidate = false;
};

inline bool isDigit(const char c) { return c >= '0' && c <= '9'; }

inline char lowerAscii(const char c) { return (c >= 'A' && c <= 'Z') ? static_cast<char>(c - 'A' + 'a') : c; }

inline bool startsWithNumberAfterOptionalV(const char* version) {
  if (version == nullptr) return false;
  if ((version[0] == 'v' || version[0] == 'V') && isDigit(version[1])) return true;
  return isDigit(version[0]);
}

inline bool containsRcMarker(const char* version) {
  if (version == nullptr) return false;
  for (const char* p = version; p[0] != '\0' && p[1] != '\0' && p[2] != '\0'; ++p) {
    if (p[0] == '-' && (p[1] == 'r' || p[1] == 'R') && (p[2] == 'c' || p[2] == 'C')) {
      return true;
    }
  }
  return false;
}

// Number right after the first case-insensitive "fluidez" marker, or
// NO_FLUIDEZ_BUILD when there is no marker, no digits, or an implausibly long number.
inline int parseFluidezBuild(const char* version) {
  if (version == nullptr) return NO_FLUIDEZ_BUILD;
  constexpr char marker[] = "fluidez";
  constexpr size_t markerLength = sizeof(marker) - 1;
  constexpr size_t maxDigits = 6;

  for (const char* p = version; *p != '\0'; ++p) {
    size_t matched = 0;
    while (matched < markerLength && lowerAscii(p[matched]) == marker[matched]) ++matched;
    if (matched != markerLength) continue;

    const char* digit = p + markerLength;
    size_t digitCount = 0;
    int value = 0;
    while (isDigit(*digit)) {
      if (++digitCount > maxDigits) return NO_FLUIDEZ_BUILD;
      value = value * 10 + (*digit - '0');
      ++digit;
    }
    return digitCount == 0 ? NO_FLUIDEZ_BUILD : value;
  }
  return NO_FLUIDEZ_BUILD;
}

inline Parsed parse(const char* version) {
  Parsed parsed;
  if (!startsWithNumberAfterOptionalV(version)) return parsed;

  const char* p = version;
  if (p[0] == 'v' || p[0] == 'V') ++p;

  size_t segmentIndex = 0;
  while (segmentIndex < SEGMENT_COUNT) {
    if (!isDigit(*p)) return parsed;

    int value = 0;
    while (isDigit(*p)) {
      value = value * 10 + (*p - '0');
      ++p;
    }
    parsed.segments[segmentIndex] = value;
    ++segmentIndex;

    if (*p != '.') break;
    ++p;
  }

  parsed.valid = true;
  parsed.releaseCandidate = containsRcMarker(version);
  parsed.fluidezBuild = parseFluidezBuild(version);
  return parsed;
}

// Positive only when latestVersion should replace currentVersion. Unparseable
// versions compare as 0, so they never trigger an update.
inline int compare(const char* latestVersion, const char* currentVersion) {
  const Parsed latest = parse(latestVersion);
  const Parsed current = parse(currentVersion);
  if (!latest.valid || !current.valid) return 0;

  for (size_t i = 0; i < SEGMENT_COUNT; ++i) {
    if (latest.segments[i] != current.segments[i]) {
      return latest.segments[i] > current.segments[i] ? 1 : -1;
    }
  }

  // A FluiDez build outranks the stock build of the same base version.
  if (latest.fluidezBuild != current.fluidezBuild) {
    return latest.fluidezBuild > current.fluidezBuild ? 1 : -1;
  }

  if (current.releaseCandidate && !latest.releaseCandidate) return 1;
  return 0;
}
}  // namespace OtaVersion
