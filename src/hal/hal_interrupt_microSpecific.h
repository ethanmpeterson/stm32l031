#ifndef HAL_INTERRUPT_MICRO_SPECIFIC_H
#define HAL_INTERRUPT_MICRO_SPECIFIC_H

#include "hal.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HAL_INTERRUPT_CHANNEL_IMU,
    HAL_INTERRUPT_CHANNEL_COUNT
} hal_interrupt_channel_E;

hal_error_E hal_interrupt_microSpecific_init(void);

#ifdef __cplusplus
}
#endif

#endif