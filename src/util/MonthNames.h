#pragma once

#include <I18n.h>

#include <cstdint>

// Localized month names for UI dates. month is 1-12; out-of-range values yield "".
inline const char* shortMonthName(const uint8_t month) {
  static constexpr StrId kIds[] = {StrId::STR_MONTH_SHORT_JAN, StrId::STR_MONTH_SHORT_FEB, StrId::STR_MONTH_SHORT_MAR,
                                   StrId::STR_MONTH_SHORT_APR, StrId::STR_MONTH_SHORT_MAY, StrId::STR_MONTH_SHORT_JUN,
                                   StrId::STR_MONTH_SHORT_JUL, StrId::STR_MONTH_SHORT_AUG, StrId::STR_MONTH_SHORT_SEP,
                                   StrId::STR_MONTH_SHORT_OCT, StrId::STR_MONTH_SHORT_NOV, StrId::STR_MONTH_SHORT_DEC};
  return month >= 1 && month <= 12 ? I18N.get(kIds[month - 1]) : "";
}

inline const char* fullMonthName(const uint8_t month) {
  static constexpr StrId kIds[] = {StrId::STR_MONTH_FULL_JAN, StrId::STR_MONTH_FULL_FEB, StrId::STR_MONTH_FULL_MAR,
                                   StrId::STR_MONTH_FULL_APR, StrId::STR_MONTH_FULL_MAY, StrId::STR_MONTH_FULL_JUN,
                                   StrId::STR_MONTH_FULL_JUL, StrId::STR_MONTH_FULL_AUG, StrId::STR_MONTH_FULL_SEP,
                                   StrId::STR_MONTH_FULL_OCT, StrId::STR_MONTH_FULL_NOV, StrId::STR_MONTH_FULL_DEC};
  return month >= 1 && month <= 12 ? I18N.get(kIds[month - 1]) : "";
}
