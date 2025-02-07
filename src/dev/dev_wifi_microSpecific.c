#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "hal_uart.h"
#include "hal_uart_microSpecific.h"

#include "dev_wifi.h"
#include "dev_wifi_microSpecific.h"

#include "hal_rtc.h"
#include "hal_rtc_microSpecific.h"

static hal_error_E dev_wifi_command_setTime(uint8_t *args, uint8_t argsLen);
static hal_error_E dev_wifi_command_setAlarm(uint8_t *args, uint8_t argsLen);
static hal_error_E dev_wifi_command_getAlarms(uint8_t *args, uint8_t argsLen);
static hal_error_E dev_wifi_command_removeAlarm(uint8_t *args, uint8_t argsLen);

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

static hal_error_E dev_wifi_command_setTime(uint8_t *args, uint8_t argsLen) {
    // Format is: Y M D hh:mm:ss
    // Could do the processing on the ESP and have it send over the formatted timeStamp

    if (argsLen > DEV_WIFI_TIME_ARGS_COUNT) {
        return HAL_ERROR_ERR;
    }

    hal_rtc_time_S timeToSet = {
        .year = args[DEV_WIFI_TIME_ARGS_YEAR],
        .month = args[DEV_WIFI_TIME_ARGS_MONTH],
        .day = args[DEV_WIFI_TIME_ARGS_DAY],
        .hour = args[DEV_WIFI_TIME_ARGS_HOUR],
        .minute = args[DEV_WIFI_TIME_ARGS_MINUTE],
        .seconds = args[DEV_WIFI_TIME_ARGS_SECONDS]
    };

    hal_rtc_setTime(&timeToSet);

    return HAL_ERROR_OK;
}

static hal_error_E dev_wifi_command_setAlarm(uint8_t *args, uint8_t argsLen) {

}

static hal_error_E dev_wifi_command_getAlarms(uint8_t *args, uint8_t argsLen) {

}

static hal_error_E dev_wifi_command_removeAlarm(uint8_t *args, uint8_t argsLen) {

}