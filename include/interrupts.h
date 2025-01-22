#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void USART2_IRQHandler(void);

void EXTI4_15_IRQHandler(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* INTERRUPTS_H */
