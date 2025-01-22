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

  return HAL_ERROR_OK;
}

