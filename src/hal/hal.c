#include "hal.h"

#include "stm32l031xx.h"
#include "stm32l0xx.h"
#include "system_stm32l0xx.h"

hal_error_E hal_init() {
  // Clock configuration and setup
  // Disable backup write protection

  // NOTE: additional logic here needed to not re-configure this through an SW
  // reset or wake from sleep. As in, the backup write protect should only get
  // set in certain cases. Or maybe since they are unchanged between resets no
  // special is handling is needed.
  PWR->CR |= PWR_CR_DBP;

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

  return HAL_ERROR_OK;
}
