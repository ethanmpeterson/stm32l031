#include "hal.h"

#include "hal_uart.h"
#include "hal_uart_microSpecific.h"

#include "dev_console.h"
#include "dev_console_microSpecific.h"

static hal_error_E dev_console_command_rtc(char **arg, uint8_t args);

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

static hal_error_E dev_console_command_rtc(char **arg, uint8_t args) {
  return HAL_ERROR_OK;
}

