#include <stdint.h>
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
static void printAlarmStatus(dev_alarm_channel_E ch);
static hal_error_E parseTimeStamp(char *timeStampString, hal_rtc_time_S *timeStamp);
static hal_error_E parseAlarmTime(char *alarmTimeString, dev_alarm_timeStamp_S *alarmTime);

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
  if (args < 1) {
    return HAL_ERROR_OK;
  }

  hal_error_E ret = HAL_ERROR_OK;
  if (!strcmp(arg[0], "alarm")) {
    if (args == 1) {
      // Print all alarm channel statuses if no channel arg is passed
      for (uint32_t i = 0; i < DEV_ALARM_CHANNEL_COUNT; i++) {
        dev_alarm_timeStamp_S timeStamp;
        dev_alarm_getAlarmTimeStamp(i, &timeStamp);
        printAlarmTime(&timeStamp);
        printAlarmStatus(i);
      }
    } else if (args >= 2) {
      uint32_t val;
      if (!strcmp(arg[1], "set") && args == 4) {
        // command format is rtc alarm set <ch> <timestamp>
        // attempt to parse the channel value
        ret = dev_console_parseDecimalDigit(arg[2], &val);
        if (ret == HAL_ERROR_OK && val < DEV_ALARM_CHANNEL_COUNT) {
          // We have a valid channel, now parse the timestamp
          dev_alarm_timeStamp_S alarmTime;
          ret = parseAlarmTime(arg[3], &alarmTime);
          if (ret == HAL_ERROR_OK) {
            hal_uart_sendString(HAL_UART_CHANNEL_COM_PORT, "New Alarm Time:\n");
            // Need a call to set the alarm time here
            dev_alarm_setAlarmTimeStamp(val, &alarmTime);
            printAlarmTime(&alarmTime);
            printAlarmStatus(val);
          }
        }
      } else {
        // if not a set command, fetch status info on the alarm
        // check the second argument for a alarm channel index
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

          printAlarmStatus(val);

        }
      }
    } else {
      // Print out all alarm channels and their time if no specific channel is provided
      for (uint32_t i = 0; i < DEV_ALARM_CHANNEL_COUNT; i++) {
        dev_alarm_timeStamp_S timeStamp;
        dev_alarm_getAlarmTimeStamp(i, &timeStamp);
        printAlarmTime(&timeStamp);
        printAlarmStatus(i);
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
    if (( (1U << i) & alarmTime->weekdayMask) ) {
      (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, '1');
    } else {
      (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, '0');
    }
  }
  (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, '\n');
}

static void printAlarmStatus(dev_alarm_channel_E ch) {
  if (dev_alarm_getStatus((dev_alarm_channel_E)ch) == DEV_ALARM_STATUS_ACTIVE) {
    hal_uart_sendString(HAL_UART_CHANNEL_COM_PORT, "ACTIVE\n");
  } else {
    hal_uart_sendString(HAL_UART_CHANNEL_COM_PORT, "INACTIVE\n");
  }
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
          if (parsedValue <= UINT8_MAX) {
            timeStamp->year = (uint8_t)parsedValue;
          } else {
            ret = HAL_ERROR_ERR;
          }
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
          } else {
            ret = HAL_ERROR_ERR;
          }
          break;

        // Fourth token is hour
        case 3:
          if (parsedValue >= 0 && parsedValue <= 23) {
            timeStamp->hour = (uint8_t)parsedValue;
          } else {
            ret = HAL_ERROR_ERR;
          }
          break;

        // Fifth token is minute
        case 4:
          if (parsedValue >= 0 && parsedValue <= 59) {
            timeStamp->minute = (uint8_t)parsedValue;
          } else {
            ret = HAL_ERROR_ERR;
          }
          break;

        // sixth token is seconds
        case 5:
          if (parsedValue >= 0 && parsedValue <= 59) {
            timeStamp->seconds = (uint8_t)parsedValue;
          } else {
            ret = HAL_ERROR_ERR;
          }
        break;

        // seventh token is weekday
        case 6:
          if (parsedValue >= 1 && parsedValue <= 7) {
            timeStamp->weekday = (uint8_t)parsedValue;
          } else {
            ret = HAL_ERROR_ERR;
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

// Alarm time presented as
// hh-mm-<weekdayMask>
// hour in 24 hour time
static hal_error_E parseAlarmTime(char *alarmTimeString, dev_alarm_timeStamp_S *alarmTime) {
  hal_error_E ret = HAL_ERROR_OK;
  char *savePointer;
  char *token = strtok_r((char *)alarmTimeString, "-", &savePointer);
  uint32_t tokNum = 0;
  // Expect 7 fields in the timestamp
  while (token != NULL && tokNum < 3) {
    // Escape if any iteration has an error set
    if (ret == HAL_ERROR_ERR) {
      break;
    }

    // Now validate the token
    uint32_t parsedValue;
    if (dev_console_parseDecimalDigit(token, &parsedValue) == HAL_ERROR_OK) {
      switch (tokNum) {
        // First token is the hour
        case 0:
          if (parsedValue >= 0 && parsedValue <= 23) {
            alarmTime->hour = (uint8_t)parsedValue;
          } else {
            ret = HAL_ERROR_ERR;
          }
          break;

        // Second token is the minute
        case 1:
          if (parsedValue >= 0 && parsedValue <= 59) {
            alarmTime->minute = (uint8_t)parsedValue;
          } else {
            ret = HAL_ERROR_ERR;
          }
          break;

        // Third token is the weekday mask
        case 2:
          if (parsedValue >= 0x00 && parsedValue <= 0x7F) {
            alarmTime->weekdayMask = (uint8_t)(parsedValue & 0x7F);
          } else {
            ret = HAL_ERROR_ERR;
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

