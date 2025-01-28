#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Shared Initialization
// Includes NVIC registration and priority setting
void interrupts_init(void);

// Define IRQ Handlers as specified in the startup assembly
void USART2_IRQHandler(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* INTERRUPTS_H */
