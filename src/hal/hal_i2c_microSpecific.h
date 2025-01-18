#ifndef HAL_I2C_MICRO_SPECIFIC_H
#define HAL_I2C_MICRO_SPECIFIC_H

#include "hal.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HAL_I2C_CHANNEL_IMU,
    HAL_I2C_CHANNEL_COUNT
} hal_i2c_channel_E;

hal_error_E hal_i2c_microSpecific_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HAL_I2C_MICRO_SPECIFIC_H */