#ifndef HAL_GPIO_MICRO_SPECIFIC_H
#define HAL_GPIO_MICRO_SPECIFIC_H

#include "hal.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HAL_GPIO_CHANNEL_LED,
    HAL_GPIO_CHANNEL_COUNT,
} hal_gpio_channel_E;

hal_error_E hal_gpio_microSpecific_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HAL_GPIO_MICRO_SPECIFIC_H */
