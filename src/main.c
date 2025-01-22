#include <stdint.h>
#include <string.h>

#include "stm32l0xx.h"

#include "system_stm32l0xx.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "portmacro.h"
#include "semphr.h"

#include "hal.h"
#include "hal_uart.h"
#include "hal_uart_microSpecific.h"

#include "hal_gpio.h"
#include "hal_gpio_microSpecific.h"

#include "hal_rtc.h"
#include "hal_rtc_microSpecific.h"

#include "dev_console.h"
#include "dev_console_microSpecific.h"

#include "interrupts.h"

// Device / Board Layer Imports

// Application Layer Imports

#define UART_BAUD_RATE 115200

// Task Handles
// These handles are used to identify the tasks and reference them.
// Suspend, resume, notify etc.
xTaskHandle blinkyTaskHandle;
xTaskHandle rtcTaskHandle;
xTaskHandle consoleTaskHandle;

extern volatile char receivedString[DEV_CONSOLE_MAX_COMMAND_LENGTH];
extern volatile bool receivedStringReady;
static void console_task(void *pvParameters) {
  for (;;) {
    // If we receive this notification, the buffer is ready to be processed
    if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) == pdTRUE) {
      // Notifications mean the buffer is good to go. Process it
      dev_console_processCommandString((char *)receivedString);
      // Clear the ready flag and buffer so the ISR can start picking up new characters
      memset((char *)receivedString, 0U, sizeof(receivedString));
      receivedStringReady = false;
    }
  }
}

static void blinky_task(void *pvParameters) {
  TickType_t lastWakeTime = xTaskGetTickCount();
  // Blink the led at 10Hz
  const TickType_t freq = 100;

  for (;;) {
    hal_gpio_pinState_E currentPinState = hal_gpio_readInputState(HAL_GPIO_CHANNEL_LED);
    hal_gpio_pinState_E nextPinState;
    if (currentPinState == HAL_GPIO_PINSTATE_ON) {
      nextPinState = HAL_GPIO_PINSTATE_OFF;
    } else {
      nextPinState = HAL_GPIO_PINSTATE_ON;
    }
    hal_gpio_setOutputState(HAL_GPIO_CHANNEL_LED, nextPinState);

    vTaskDelayUntil(&lastWakeTime, freq);
  }
}

static void rtc_task(void *pvParameters) {
  TickType_t lastWakeTime = xTaskGetTickCount();
  const TickType_t freq = 1000;

  for (;;) {
    vTaskDelayUntil(&lastWakeTime, freq);
  }
}

int main(void) {
  (void)hal_init();

  (void)hal_rtc_microSpecific_init();
  (void)hal_gpio_microSpecific_init();
  (void)hal_uart_microSpecific_init();

  hal_rtc_time_S initialTime = {
      .year = 0,  // 2000
      .month = 9, // September
      .day = 14,
      .weekday = 4,
      .hour = 13, // 1PM (24H format)
      .minute = 32,
      .seconds = 54
  };

  hal_rtc_setTime(&initialTime);

  // Device layer init
  (void)dev_console_microSpecific_init();

  interrupts_init();

  // Task names must be < 16
  xTaskCreate(blinky_task, "blinky", 50, NULL, tskIDLE_PRIORITY, &blinkyTaskHandle);
  xTaskCreate(rtc_task, "rtc", 50, NULL, tskIDLE_PRIORITY + 1, &rtcTaskHandle);
  // Set console task as highest priority so nothing hangs
  xTaskCreate(console_task, "console", 50, NULL, tskIDLE_PRIORITY + 2, &consoleTaskHandle);

  vTaskStartScheduler();

  return 0;
}
