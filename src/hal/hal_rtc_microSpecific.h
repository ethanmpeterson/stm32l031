#ifndef HAL_RTC_MICRO_SPECIFIC_H
#define HAL_RTC_MICRO_SPECIFIC_H

#include "hal.h"
#include "hal_rtc.h"

#ifdef __cplusplus
extern "C" {
#endif

hal_error_E hal_rtc_microSpecific_init(void);
hal_error_E hal_rtc_microSpecific_getTime(hal_rtc_time_S *time);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HAL_RTC_MICRO_SPECIFIC_H */