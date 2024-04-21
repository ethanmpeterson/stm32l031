#include "stm32l031xx.h"
#include "stm32l0xx.h"
#include "system_stm32l0xx.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include <stdint.h>

/* #define SET_BIT_U32(value, bit_idx) ((uint32_t)value | (uint32_t)(1 << bit_idx)) */
/* #define CLEAR_BIT_U32(value, bit_idx) ((uint32_t)value & (uint32_t)(~(1 << bit_idx))) */

#define LED_PIN 3
volatile uint32_t ticker = 0;

static void myTask1(void *pvParameters) {
  for (;;) {
    if (ticker == 100000) {
      ticker = 0;
      GPIOB->ODR ^= (1 << LED_PIN);
    } else {
      ticker++;
    }
  }
}

int main (void) {
  // Clock configuration and setup
  PWR->CR |= (1 << 8);

  // Enable HSI, PLL and set prescalers
  RCC->CR |= RCC_CR_HSION;

  // Set the PLL prescaling before enabling the PLL
  RCC->CFGR |= RCC_CFGR_PLLMUL4;
  RCC->CFGR |= RCC_CFGR_PLLDIV2;
  RCC->CFGR |= RCC_CFGR_PLLSRC_HSI;
  // Enable the PLL
  RCC->CR |= RCC_CR_PLLON;

  // Set up the APB and AHB prescalers. The init values already divide by 1 but
  // in the spirit of being explicit
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

  // Wait for the PLL to be locked and then select it as the system clock
  while (!(RCC->CR & RCC_CR_PLLRDY));

  // Wait for flash to be ready
  FLASH->ACR |= FLASH_ACR_LATENCY;
  while ((FLASH->ACR & FLASH_ACR_LATENCY) == 0);

  // Enable the PLL as system input clock
  RCC->CFGR |= RCC_CFGR_SW_PLL;

  // READ SWS field to ensure everything is brought up before running another clock update
  while ((RCC->CFGR & RCC_CFGR_SWS_PLL) == 0);

  SystemCoreClockUpdate();
  volatile uint32_t tick = 0;
  // Enable the GPIO clock
  RCC->IOPENR |= RCC_IOPENR_IOPBEN;

  // Inits to all 1s clear it so that all pins are input
  GPIOB->MODER = 0U;

  // Set PB3 to output
  GPIOB->MODER |= GPIO_MODER_MODE3_0;

  portBASE_TYPE creation_return;
  xTaskHandle xHandleTask1;

  /* Start the reg test tasks - defined in this file. */
  xTaskCreate(myTask1, "Task1", 50, NULL, tskIDLE_PRIORITY + 1, &xHandleTask1);

  vTaskStartScheduler();

  return 0;
}
