#ifndef HAL_UART_MICRO_SPECIFIC_H
#define HAL_UART_MICRO_SPECIFIC_H

#include "hal.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HAL_UART_CHANNEL_COM_PORT,
    HAL_UART_CHANNEL_COUNT,
} hal_uart_channel_E;

hal_error_E hal_uart_microSpecific_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HAL_UART_MICRO_SPECIFIC_H */
