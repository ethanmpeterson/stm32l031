#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "hal_uart.h"
#include "hal_uart_microSpecific.h"

#include "dev_wifi.h"
#include "dev_wifi_microSpecific.h"

#include "hal_rtc.h"
#include "hal_rtc_microSpecific.h"

static hal_error_E dev_wifi_command_setTime(char **arg, uint32_t args);
static hal_error_E dev_wifi_command_setAlarm(char **arg, uint32_t args);
static hal_error_E dev_wifi_command_getAlarms(char **arg, uint32_t args);
static hal_error_E dev_wifi_command_removeAlarm(char **arg, uint32_t args);

static const dev_wifi_command_S dev_wifi_commands[] = {
    {
        .id = DEV_WIFI_COMMAND_SET_TIME,
        .callback = dev_wifi_command_setTime,
    },
    {
        .id = DEV_WIFI_COMMAND_SET_ALARM,
        .callback = dev_wifi_command_setAlarm,
    },
    {
        .id = DEV_WIFI_COMMAND_GET_ALARMS,
        .callback = dev_wifi_command_getAlarms,
    },
    {
        .id = DEV_WIFI_COMMAND_REMOVE_ALARM,
        .callback = dev_wifi_command_removeAlarm,
    }
};

static const dev_wifi_config_S dev_wifi_config = {
    .wifiPort = HAL_UART_CHANNEL_ESP_PORT,
    .commands = dev_wifi_commands,
    .commandCount = sizeof(dev_wifi_commands)
};

hal_error_E dev_wifi_microSpecific_init(void) {
    return dev_wifi_init(&dev_wifi_config);
}

static hal_error_E dev_wifi_command_setTime(char **arg, uint32_t args) {
    // Format is: Y M D hh:mm:ss
    // Could do the processing on the ESP and have it send over the formatted timeStamp

    if (args > DEV_WIFI_TIME_ARGS_COUNT) {
        return HAL_ERROR_ERR;
    }

    hal_rtc_time_S timeToSet = {
        .year = arg[DEV_WIFI_TIME_ARGS_YEAR],
        .month = arg[DEV_WIFI_TIME_ARGS_MONTH],
        .day = arg[DEV_WIFI_TIME_ARGS_DAY],
        .hour = arg[DEV_WIFI_TIME_ARGS_HOUR],
        .minute = arg[DEV_WIFI_TIME_ARGS_MINUTE],
        .seconds = arg[DEV_WIFI_TIME_ARGS_SECONDS]
    };

    hal_rtc_setTime(&timeToSet);

    return HAL_ERROR_OK;
}

static hal_error_E dev_wifi_command_setAlarm(char **arg, uint32_t args) {

}

static hal_error_E dev_wifi_command_getAlarms(char **arg, uint32_t args) {

}

static hal_error_E dev_wifi_command_removeAlarm(char **arg, uint32_t args) {

}