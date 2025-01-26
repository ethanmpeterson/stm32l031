#include <string.h>
#include "hal.h"

#include "dev_alarm.h"
#include "dev_alarm_microSpecific.h"

// alarm channel definitions
static const dev_alarm_channelConfig_S dev_alarm_channelConfigs[DEV_ALARM_CHANNEL_COUNT] = {
  [DEV_ALARM_CHANNEL_PRIMARY] = {
    .alarmTime = {
      // 7:30 AM Alarm from Monday to Friday
      .hour = 7,
      .minute = 30,
      .weekdayMask = 0x1F
    },
    .callback = NULL,
  },

  [DEV_ALARM_CHANNEL_SECONDARY] = {
    .alarmTime = {
      // 9:30 AM Alarm on Saturday and Sunday
      .hour = 9,
      .minute = 30,
      .weekdayMask = 0x60
    },
    .callback = NULL,
  },

};

static const dev_alarm_config_S dev_alarm_config = {
  .channels = dev_alarm_channelConfigs
};

void dev_alarm_microSpecific_init(void) {
  // Initialize shared module
  dev_alarm_init(&dev_alarm_config);
}

