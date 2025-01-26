#ifndef DEV_ALARM_MICRO_SPECIFIC_H
#define DEV_ALARM_MICRO_SPECIFIC_H

#include "hal.h"
#ifdef __cplusplus
extern "C" {
#endif

// For now support just two alarms. (weekend and weekday) In future, we could
// make this dynamically allocated to add remove them through the console or
// other interface.
typedef enum {
  DEV_ALARM_CHANNEL_PRIMARY,
  DEV_ALARM_CHANNEL_SECONDARY,
  DEV_ALARM_CHANNEL_COUNT,
  DEV_ALARM_CHANNEL_UNKNOWN
} dev_alarm_channel_E;

void dev_alarm_microSpecific_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DEV_ALARM_MICRO_SPECIFIC_H */
