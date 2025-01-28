#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "dev_alarm.h"
#include "dev_alarm_microSpecific.h"
#include "hal.h"

#include "hal_rtc.h"
#include "hal_rtc_microSpecific.h"

#include "hal_uart.h"
#include "hal_uart_microSpecific.h"

#include "dev_console.h"
#include "dev_console_microSpecific.h"

static hal_error_E dev_console_command_rtc(char **arg, uint32_t args);

// Commands and callbacks
static const dev_console_command_S dev_console_commands[] = {
  {
    .prefix = "rtc",
    .callback = dev_console_command_rtc
  }
};

static const dev_console_config_S dev_console_config = {
  .consolePort = HAL_UART_CHANNEL_COM_PORT,
  .commands = dev_console_commands,
  .commandCount = sizeof(dev_console_commands)
};

hal_error_E dev_console_microSpecific_init(void) {
  return dev_console_init(&dev_console_config);
}

static hal_error_E dev_console_command_rtc(char **arg, uint32_t args) {
  // Print a timestamp the 'rtc' command is run
  hal_rtc_time_S currentTime;
  (void)hal_rtc_getTime(&currentTime);
  char secondsTens = (char)((currentTime.seconds / 10) % 10) + '0';
  char secondsUnits = (char)(currentTime.seconds % 10) + '0';
  char minutesTens = (char)((currentTime.minute / 10) % 10) + '0';
  char minutesUnits = (char)(currentTime.minute % 10) + '0';
  char hoursTens = (char)((currentTime.hour / 10) % 10) + '0';
  char hoursUnits = (char)(currentTime.hour % 10) + '0';
  char dayTens = (char)((currentTime.day / 10) % 10) + '0';
  char dayUnits = (char)(currentTime.day % 10) + '0';
  char monthTens = (char)((currentTime.month / 10) % 10) + '0';
  char monthUnits = (char)(currentTime.month % 10) + '0';
  char yearTens = (char)((currentTime.year / 10) % 10) + '0';
  char yearUnits = (char)(currentTime.year % 10) + '0';
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, '2');
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, '0');
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, yearTens);
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, yearUnits);
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, '/');
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, monthTens);
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, monthUnits);
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, '/');
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, dayTens);
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, dayUnits);

  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, '-');

  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, hoursTens);
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, hoursUnits);
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, ':');
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, minutesTens);
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, minutesUnits);
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, ':');
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, secondsTens);
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, secondsUnits);
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, '\n');

  // Handle the CLI command
  if (args <= 1) {
    return HAL_ERROR_OK;
  }

  hal_error_E ret = HAL_ERROR_OK;
  if (!strcmp(arg[0], "alarm")) {
    if (args >= 2) {
      // check the second argument for a alarm channel index
      uint32_t val;
      ret = dev_console_parseDecimalDigit(arg[1], &val);
      if (ret == HAL_ERROR_OK && val < DEV_ALARM_CHANNEL_COUNT) {
        // Print alarm info
        dev_alarm_timeStamp_S alarmTime;
        if (dev_alarm_getAlarmTimeStamp((dev_alarm_channel_E)val, &alarmTime) == HAL_ERROR_OK) {
          char minutesTens = (char)((alarmTime.minute / 10) % 10) + '0';
          char minutesUnits = (char)(alarmTime.minute % 10) + '0';
          char hoursTens = (char)((alarmTime.hour / 10) % 10) + '0';
          char hoursUnits = (char)(alarmTime.hour % 10) + '0';
          (void)hal_uart_sendString(HAL_UART_CHANNEL_COM_PORT, "Alarm Time: ");

          (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, hoursTens);
          (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, hoursUnits);
          (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, ':');
          (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, minutesTens);
          (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, minutesUnits);

          (void)hal_uart_sendString(HAL_UART_CHANNEL_COM_PORT, "\nWeekday Mask: ");
          // Print alarm bitmask
          for (uint8_t i = 0; i < 7; i++) {
            if ((1U << i & alarmTime.weekdayMask)) {
              (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, '1');
            } else {
              (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, '0');
            }
          }
          (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, '\n');
        } else {
          ret = HAL_ERROR_ERR;
          hal_uart_sendString(HAL_UART_CHANNEL_COM_PORT, "Unable to fetch alarm time for this channel\n");
        }

        if (dev_alarm_getStatus((dev_alarm_channel_E)val) == DEV_ALARM_STATUS_ACTIVE) {
          hal_uart_sendString(HAL_UART_CHANNEL_COM_PORT, "ACTIVE\n");
        } else {
          hal_uart_sendString(HAL_UART_CHANNEL_COM_PORT, "INACTIVE\n");
        }
      }
    }
  }

  return HAL_ERROR_OK;
}

