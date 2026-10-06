#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>

// Format a known date using the persisted date-format values shared by settings and HalClock.
// shortMonthNames/fullMonthNames: optional 12-entry localized tables (January first) for the
// long formats; a missing table or entry falls back to English.
inline bool formatDateParts(char* buf, const size_t bufSize, const uint16_t year, const uint8_t month,
                            const uint8_t day, const uint8_t dateFormat, const char numericSeparator = '/',
                            const char* const* shortMonthNames = nullptr, const char* const* fullMonthNames = nullptr) {
  if (bufSize == 0 || month < 1 || month > 12 || day < 1 || day > 31) return false;

  static constexpr const char* monthNames[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                               "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  static constexpr const char* englishFullMonthNames[] = {"January",   "February", "March",    "April",
                                                          "May",       "June",     "July",     "August",
                                                          "September", "October",  "November", "December"};
  const char* shortMonth =
      shortMonthNames && shortMonthNames[month - 1] ? shortMonthNames[month - 1] : monthNames[month - 1];
  const char* fullMonth =
      fullMonthNames && fullMonthNames[month - 1] ? fullMonthNames[month - 1] : englishFullMonthNames[month - 1];
  const char separator = numericSeparator == '.' || numericSeparator == '-' ? numericSeparator : '/';
  int length = 0;
  switch (dateFormat) {
    case 1:  // Day month year, long
      length = std::snprintf(buf, bufSize, "%02u %s %u", day, shortMonth, year);
      break;
    case 2:  // Month day year, numeric
      length = std::snprintf(buf, bufSize, "%02u%c%02u%c%u", month, separator, day, separator, year);
      break;
    case 3:  // Day month year, numeric
      length = std::snprintf(buf, bufSize, "%02u%c%02u%c%u", day, separator, month, separator, year);
      break;
    case 4:  // Year month day, numeric
      length = std::snprintf(buf, bufSize, "%u%c%02u%c%02u", year, separator, month, separator, day);
      break;
    case 5:  // Month day, numeric
      length = std::snprintf(buf, bufSize, "%02u%c%02u", month, separator, day);
      break;
    case 6:  // Day month, numeric
      length = std::snprintf(buf, bufSize, "%02u%c%02u", day, separator, month);
      break;
    case 7:  // Month day, long
      length = std::snprintf(buf, bufSize, "%s %02u", fullMonth, day);
      break;
    case 8:  // Day month, long
      length = std::snprintf(buf, bufSize, "%02u %s", day, fullMonth);
      break;
    case 0:  // Month day year, long
    default:
      length = std::snprintf(buf, bufSize, "%s %02u, %u", shortMonth, day, year);
      break;
  }
  return length >= 0 && static_cast<size_t>(length) < bufSize;
}
