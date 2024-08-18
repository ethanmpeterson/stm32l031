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

// Device / Board Layer Imports

// Application Layer Imports

#define UART_BAUD_RATE 115200

#define LED_PIN 3
volatile uint32_t ticker = 0;

static void blinky_task(void *pvParameters) {
  for (;;) {
    GPIOB->ODR ^= (1 << LED_PIN);
    vTaskDelay(100);
  }
}

static void uart_task(void *pvParameters) {
  for (;;) {
    // Transfer the byte if the peripheral is available
    (void)hal_uart_sendByte(HAL_UART_CHANNEL_COM_PORT, 'e');

    // Higher priority task than the blinky
    // Give some time for the blinky to run
    vTaskDelay(1000);
  }
}

int main(void) {
  (void)hal_init();

  hal_uart_microSpecific_init();

  // Enable the GPIO clock
  RCC->IOPENR |= RCC_IOPENR_IOPBEN;

  // Inits to all 1s clear it so that all pins are input
  GPIOB->MODER = 0U;

  // Set PB3 to output
  GPIOB->MODER |= GPIO_MODER_MODE3_0;

  xTaskHandle xHandleTask1;
  xTaskCreate(blinky_task, "blinky", 50, NULL, tskIDLE_PRIORITY, &xHandleTask1);
  xTaskCreate(uart_task, "uart", 50, NULL, tskIDLE_PRIORITY + 1, &xHandleTask1);

  vTaskStartScheduler();

  return 0;
}
