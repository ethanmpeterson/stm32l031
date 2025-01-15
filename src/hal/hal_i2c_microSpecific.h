#ifndef HAL_I2C_MICRO_SPECIFIC_H
#define HAL_I2C_MICRO_SPECIFIC_H

#include "hal.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HAL_I2C_ADDRESS_MODE_7_BIT,
    HAL_I2C_ADDRESS_MODE_10_BIT
} hal_i2c_address_mode;

typedef enum {
    HAL_I2C_SPEED_MODE_STANDARD,
    HAL_I2C_SPEED_MODE_FAST,
    HAL_I2C_SPEED_MODE_FAST_PLUS
} hal_i2c_speed_mode;

typedef enum {
    HAL_I2C_MODE_MASTER_TRANSMITTER,
    HAL_I2C_MODE_MASTER_RECEIVER,
    HAL_I2C_MODE_SLAVE_TRANSMITTER,
    HAL_I2C_MODE_SLAVE_RECEIVER
} hal_i2c_mode;

hal_error_E hal_i2c_microSpecific_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HAL_I2C_MICRO_SPECIFIC_H */