#include <stdint.h>

#include "stm32l031xx.h"
#include "stm32l0xx.h"

#include "system_stm32l0xx.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "hal.h"
#include "hal_uart.h"
#include "hal_uart_microSpecific.h"

#include "hal_gpio.h"
#include "hal_gpio_microSpecific.h"

#include "hal_rtc.h"
#include "hal_rtc_microSpecific.h"

// Device / Board Layer Imports

// Application Layer Imports

#define UART_BAUD_RATE 115200

static void blinky_task(void *pvParameters) {
  for (;;) {
    hal_gpio_pinState_E currentPinState = hal_gpio_readInputState(HAL_GPIO_CHANNEL_LED);
    hal_gpio_pinState_E nextPinState;
    if (currentPinState == HAL_GPIO_PINSTATE_ON) {
      nextPinState = HAL_GPIO_PINSTATE_OFF;
    } else {
      nextPinState = HAL_GPIO_PINSTATE_ON;
    }
    hal_gpio_setOutputState(HAL_GPIO_CHANNEL_LED, nextPinState);

    vTaskDelay(100);
  }
}

static void uart_task(void *pvParameters) {
  for (;;) {
    // Transfer the byte if the peripheral is available
    hal_rtc_time_S currentTime;
    currentTime.seconds = 0;
    (void)hal_rtc_microSpecific_getTime(&currentTime);
    char secondsTens = (char)((currentTime.seconds / 10) % 10) + '0';
    char secondsUnits = (char)(currentTime.seconds % 10) + '0';
    char minutesTens = (char)((currentTime.minute / 10) % 10) + '0';
    char minutesUnits = (char)(currentTime.minute % 10) + '0';
    char hoursTens = (char)((currentTime.hour / 10) % 10) + '0';
    char hoursUnits = (char)(currentTime.hour % 10) + '0';

    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, hoursTens);
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, hoursUnits);
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, ':');
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, minutesTens);
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, minutesUnits);
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, ':');
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, secondsTens);
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, secondsUnits);
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, '\n');

    /* (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, ':'); */

    // Higher priority task than the blinky
    // Give some time for the blinky to run
    vTaskDelay(1000);
  }
}

int main(void) {
  (void)hal_init();

  (void)hal_rtc_microSpecific_init();
  (void)hal_gpio_microSpecific_init();
  (void)hal_uart_microSpecific_init();

  // These handles are used to identify the tasks and reference them.
  // Suspend, resume, notify etc.
  xTaskHandle blinkyTaskHandle;
  xTaskHandle uartTaskHandle;

  xTaskCreate(blinky_task, "blinky", 50, NULL, tskIDLE_PRIORITY, &blinkyTaskHandle);
  xTaskCreate(uart_task, "uart", 50, NULL, tskIDLE_PRIORITY + 1, &uartTaskHandle);

  vTaskStartScheduler();

  return 0;
}
