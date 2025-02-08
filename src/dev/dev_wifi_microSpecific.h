#ifndef DEV_WIFI_MICRO_SPECIFIC_H
#define DEV_WIFI_MICRO_SPECIFIC_H

#include "hal.h"
#ifdef __cplusplus
extern "C" {
#endif

hal_error_E dev_wifi_microSpecific_init(void);

typedef enum {

    DEV_WIFI_TIME_ARGS_YEAR,
    DEV_WIFI_TIME_ARGS_MONTH,
    DEV_WIFI_TIME_ARGS_DAY,
    DEV_WIFI_TIME_ARGS_HOUR,
    DEV_WIFI_TIME_ARGS_MINUTE,
    DEV_WIFI_TIME_ARGS_SECONDS,
    DEV_WIFI_TIME_ARGS_COUNT

} dev_wifi_time_args_E;

#ifdef __cplusplus
}
#endif

#endif