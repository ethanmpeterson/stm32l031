#include "hal_uart.h"
#include "hal.h"

#include "stm32l031xx.h"
#include "stm32l0xx.h"
#include "system_stm32l0xx.h"

#include "shared_defs.h"

hal_error_E hal_uart_init(hal_uart_config_S const * const config) {
  // CONFIGURE UART
  // PA2 Tx PA15 RX on this board
  // Both need to be set to alternate function 4 (AF4) according to the
  // datasheet

  // Enable USART2 clock
  RCC->APB1ENR |= 1 << RCC_APB1ENR_USART2EN_Pos;

  RCC->IOPENR |= RCC_IOPENR_IOPAEN;
  // Put PA2 in AF4
  GPIOA->AFR[0] |= 4 << GPIO_AFRL_AFSEL2_Pos;
  // Put PA15 in AF4
  GPIOA->AFR[1] |= 4 << GPIO_AFRH_AFSEL15_Pos;

  // Clear GPIO pins we are going to be using only
  // Avoids interference with SWD debug lines on PORTA
  GPIOA->MODER = CLEAR_BIT_U32(GPIOA->MODER, GPIO_MODER_MODE2_Pos);
  GPIOA->MODER = CLEAR_BIT_U32(GPIOA->MODER, GPIO_MODER_MODE2_Pos + 1);
  GPIOA->MODER = CLEAR_BIT_U32(GPIOA->MODER, GPIO_MODER_MODE15_Pos);
  GPIOA->MODER = CLEAR_BIT_U32(GPIOA->MODER, GPIO_MODER_MODE15_Pos + 1);

  // 10 is alternate function mode. Will set this for PA2 and PA15
  GPIOA->MODER |= (2 << GPIO_MODER_MODE2_Pos) | (2 << GPIO_MODER_MODE15_Pos);

  // Use default 16 bit oversample
  USART2->BRR = (SystemCoreClock / 115200);
  USART2->CR1 = USART_CR1_TE | USART_CR1_UE;

  return HAL_ERROR_OK;
}
