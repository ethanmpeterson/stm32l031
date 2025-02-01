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
static void printCurrentTime(void);
static void printAlarmTime(dev_alarm_timeStamp_S *alarmTime);
static hal_error_E parseTimeStamp(char *timeStampString, hal_rtc_time_S *timeStamp);

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
  // Print a timestamp when the 'rtc' command is run
  printCurrentTime();

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
          printAlarmTime(&alarmTime);
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
  } else if (!strcmp(arg[0], "set") && args == 2) {
    // Parse a timestamp and set new RTC time
    hal_rtc_time_S timeStamp;
    ret = parseTimeStamp(arg[1], &timeStamp);
    // Update the time if we successfully parsed
    if (ret == HAL_ERROR_OK) {
      if (hal_rtc_setTime(&timeStamp) == HAL_ERROR_OK) {
        hal_uart_sendString(HAL_UART_CHANNEL_COM_PORT, "New Time:");
        printCurrentTime();
      }
    }
  } else {
    ret = HAL_ERROR_ERR;
  }


  return ret;
}

static void printCurrentTime() {
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

}

static void printAlarmTime(dev_alarm_timeStamp_S *alarmTime) {
  char minutesTens = (char)((alarmTime->minute / 10) % 10) + '0';
  char minutesUnits = (char)(alarmTime->minute % 10) + '0';
  char hoursTens = (char)((alarmTime->hour / 10) % 10) + '0';
  char hoursUnits = (char)(alarmTime->hour % 10) + '0';
  (void)hal_uart_sendString(HAL_UART_CHANNEL_COM_PORT, "Alarm Time: ");

  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, hoursTens);
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, hoursUnits);
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, ':');
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, minutesTens);
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, minutesUnits);

  (void)hal_uart_sendString(HAL_UART_CHANNEL_COM_PORT, "\nWeekday Mask: ");
  // Print alarm bitmask
  for (uint8_t i = 0; i < 7; i++) {
    if ((1U << i & alarmTime->weekdayMask)) {
      (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, '1');
    } else {
      (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, '0');
    }
  }
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, '\n');
}

// Example Timestamp
// year 2000+ is implied
// 25-01-31-22-41-38-THU
// yy-mm-dd-hh-mm-ss-weekday
// Weekday represented in hal format
// 1 == Monday, 2 == Tuesday etc.
static hal_error_E parseTimeStamp(char *timeStampString, hal_rtc_time_S *timeStamp) {
  hal_error_E ret = HAL_ERROR_OK;
  char *savePointer;
  char *token = strtok_r((char *)timeStampString, "-", &savePointer);
  uint32_t tokNum = 0;
  // Expect 7 fields in the timestamp
  while (token != NULL && tokNum < 7) {
    // Now validate the token
    uint32_t parsedValue;
    if (dev_console_parseDecimalDigit(token, &parsedValue) == HAL_ERROR_OK) {
      switch (tokNum) {
        // First token is the year
        case 0:
            timeStamp->year = (uint8_t)parsedValue;
          break;

        // Second token is the month
        case 1:
          if (parsedValue >= 1 && parsedValue <= 12) {
            timeStamp->month = (uint8_t)parsedValue;
          }
          break;

        // Third token is the day
        case 2:
          // Only basic bounds checking is done, up to the command sender to not
          // send bogus dates. E.g February 30th. Easily avoided with a small py
          // script to generate the timestamp.
          if (parsedValue >= 1 && parsedValue <= 31) {
            timeStamp->day = (uint8_t)parsedValue;
          }
          break;

        // Fourth token is hour
        case 3:
          if (parsedValue >= 0 && parsedValue <= 23) {
            timeStamp->hour = (uint8_t)parsedValue;
          }
          break;

        // Fifth token is minute
        case 4:
          if (parsedValue >= 0 && parsedValue <= 59) {
            timeStamp->minute = (uint8_t)parsedValue;
          }
          break;

        // sixth token is seconds
        case 5:
          if (parsedValue >= 0 && parsedValue <= 59) {
            timeStamp->seconds = (uint8_t)parsedValue;
          }
        break;

        // seventh token is weekday
        case 6:
          if (parsedValue >= 1 && parsedValue <= 7) {
            timeStamp->weekday = (uint8_t)parsedValue;
          }
        break;

        default:
          ret = HAL_ERROR_ERR;
        break;
      }
    } else {
      // break out of the loop and return an error
      ret = HAL_ERROR_ERR;
      break;
    }

    // Fetch the next token
    token = strtok_r(NULL, "-", &savePointer);
    tokNum++;
  }

  return ret;
}
