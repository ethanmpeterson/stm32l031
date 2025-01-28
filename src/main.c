#include <stdint.h>
#include <string.h>

#include "FreeRTOSConfig.h"
#include "dev_alarm.h"
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
#include "dev_alarm_microSpecific.h"

#include "interrupts.h"

// Device / Board Layer Imports

// Application Layer Imports

#define UART_BAUD_RATE 115200

// Task Handles
// These handles are used to identify the tasks and reference them.
// Suspend, resume, notify etc.
xTaskHandle slowTaskHandle;
xTaskHandle fastTaskHandle;
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

static void task_100ms(void *pvParameters) {
  TickType_t lastWakeTime = xTaskGetTickCount();
  // Blink the led at 10Hz
  const TickType_t freq = 100;

  for (;;) {
    // Blinky
    hal_gpio_pinState_E currentPinState = hal_gpio_readInputState(HAL_GPIO_CHANNEL_LED);
    hal_gpio_pinState_E nextPinState;
    if (currentPinState == HAL_GPIO_PINSTATE_ON) {
      nextPinState = HAL_GPIO_PINSTATE_OFF;
    } else {
      nextPinState = HAL_GPIO_PINSTATE_ON;
    }
    hal_gpio_setOutputState(HAL_GPIO_CHANNEL_LED, nextPinState);

    // Device layer state machines
    dev_alarm_runLoop();

    // App layer state machines

    vTaskDelayUntil(&lastWakeTime, freq);
  }
}

static void task_10ms(void *pvParameters) {
  TickType_t lastWakeTime = xTaskGetTickCount();
  // Blink the led at 100Hz
  const TickType_t freq = 10;

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
      .weekday = 4, // Thursday
      .hour = 7, // 1PM (24H format)
      .minute = 29,
      .seconds = 30
  };

  hal_rtc_setTime(&initialTime);

  // Device layer init
  (void)dev_console_microSpecific_init();
  dev_alarm_microSpecific_init();

  interrupts_init();

  // Stack is max 128 words given FreeRTOS heap size of 4096
  // Task names must be < 16
  xTaskCreate(task_100ms, "task_100ms", 32, NULL, tskIDLE_PRIORITY, &slowTaskHandle);
  xTaskCreate(task_10ms, "task_10ms", 32, NULL, tskIDLE_PRIORITY, &fastTaskHandle);
  // Set console task as highest priority so nothing hangs
  xTaskCreate(console_task, "console", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &consoleTaskHandle);

  vTaskStartScheduler();

  return 0;
}
