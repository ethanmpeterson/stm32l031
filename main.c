#include "stm32l031xx.h"
#include "stm32l0xx.h"

int main (void) {
  SystemCoreClockUpdate();

  // Clock configuration and setup
  PWR->CR |= (1 << 8);

  // Enable HSI
  RCC->CR |= (1 << RCC_CR_HSION_Pos);
  RCC->CR |= (1 << RCC_CR_PLLON_Pos);

  // Next setup downstream peripheral clocks
  /* RCC->CFGR |= RCC_CFGR_PLLMUL */
  RCC->CFGR |= 1;

  volatile uint32_t bro = 0;
  // Enable the GPIO clock
  RCC->IOPENR |= (1 << RCC_IOPENR_IOPBEN_Pos);
  // Inits to all 1s clear it so that all pins are input
  GPIOB->MODER = 0U;

  // Set PB3 to output
  // NOTE: needs lots of clean up
  GPIOB->MODER |= (1 << 6);
  uint32_t odr_init_value = GPIOB->ODR;

  for (;;) {
    if (bro == 1000000) {
      bro = 0;
      GPIOB->ODR ^= (1 << 3);
    }
    bro++;
  }

  return 0;
}
