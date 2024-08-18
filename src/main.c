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
    (void)hal_uart_sendByte(HAL_UART_CHANNEL_COM_PORT, 'n');

    // Higher priority task than the blinky
    // Give some time for the blinky to run
    vTaskDelay(1000);
  }
}

int main(void) {
  (void)hal_init();

  (void)hal_gpio_microSpecific_init();
  (void)hal_uart_microSpecific_init();

  // Enable the GPIO clock
  RCC->IOPENR |= RCC_IOPENR_IOPBEN;

  // Inits to all 1s clear it so that all pins are input
  GPIOB->MODER = 0U;

  // Set PB3 to output
  GPIOB->MODER |= GPIO_MODER_MODE3_0;

  xTaskHandle xHandleTask1;
  xTaskCreate(blinky_task, "blinky", 50, NULL, tskIDLE_PRIORITY,
              &xHandleTask1);
  xTaskCreate(uart_task, "uart", 50, NULL, tskIDLE_PRIORITY + 1,
              &xHandleTask1);

  vTaskStartScheduler();

  return 0;
}
