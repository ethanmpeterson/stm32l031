#include <stdbool.h>

#include "hal.h"
#include "hal_gpio.h"

#include "hal_gpio_microSpecific.h"
#include "stm32l031xx.h"
#include "stm32l0xx.h"
#include "system_stm32l0xx.h"

#include "shared_defs.h"

#define LED_PIN 3

static hal_error_E hal_gpio_microSpecific_initLEDChannel(void);
static hal_gpio_pinState_E hal_gpio_microSpecific_readLEDPin(void);
static hal_error_E
hal_gpio_microSpecific_setLEDPin(hal_gpio_pinState_E pinState);
static hal_error_E
hal_gpio_microSpecific_setLEDPinMode(hal_gpio_pinMode_E pinMode);
static hal_error_E
hal_gpio_microSpecific_setLEDPullMode(hal_gpio_pullMode_E pullMode);

static const hal_gpio_channelConfig_S
    hal_gpio_channelConfigs[HAL_GPIO_CHANNEL_COUNT] = {

        [HAL_GPIO_CHANNEL_LED] =
            {
                .initChannel = hal_gpio_microSpecific_initLEDChannel,
                .readPinState = hal_gpio_microSpecific_readLEDPin,
                .setPinState = hal_gpio_microSpecific_setLEDPin,
                .setPinMode = hal_gpio_microSpecific_setLEDPinMode,
                .setPullMode = hal_gpio_microSpecific_setLEDPullMode,
            },
};

// Config
static hal_gpio_config_S hal_gpio_config = {
  .channels     = hal_gpio_channelConfigs,
  .channelCount = HAL_GPIO_CHANNEL_COUNT,
};

hal_error_E hal_gpio_microSpecific_init(void) {
    // Any other device specific initialization could also be placed here
    // call shared code initialization
    return hal_gpio_init(&hal_gpio_config);
}

static hal_error_E hal_gpio_microSpecific_initLEDChannel(void) {
  // Enable the GPIO clock
  RCC->IOPENR |= RCC_IOPENR_IOPBEN;

  // Inits to all 1s clear it so that all pins are input
  GPIOB->MODER = 0U;

  // Set PB3 to output
  GPIOB->MODER |= GPIO_MODER_MODE3_0;

  return HAL_ERROR_OK;
}

static hal_gpio_pinState_E hal_gpio_microSpecific_readLEDPin(void) {
  // TODO: replace all similar statements with an "isBitSet" helper macro
  bool enabled = (GPIOB->IDR & (1U << LED_PIN));
  return enabled ? HAL_GPIO_PINSTATE_ON : HAL_GPIO_PINSTATE_OFF;
}

static hal_error_E hal_gpio_microSpecific_setLEDPin(hal_gpio_pinState_E pinState) {
  hal_error_E ret = HAL_ERROR_OK;

  switch (pinState) {
    case HAL_GPIO_PINSTATE_ON:
      GPIOB->ODR = SET_BIT_U32(GPIOB->ODR, LED_PIN);
      break;
    case HAL_GPIO_PINSTATE_OFF:
      GPIOB->ODR = CLEAR_BIT_U32(GPIOB->ODR, LED_PIN);
      break;

    case HAL_GPIO_PINSTATE_COUNT:
    case HAL_GPIO_PINSTATE_UNKNOWN:
    default:
      ret = HAL_ERROR_ERR;
      break;
  }

  return ret;
}

static hal_error_E hal_gpio_microSpecific_setLEDPinMode(hal_gpio_pinMode_E pinMode) {
  return HAL_ERROR_OK;
}

static hal_error_E hal_gpio_microSpecific_setLEDPullMode(hal_gpio_pullMode_E pullMode) {
  return HAL_ERROR_OK;
}

