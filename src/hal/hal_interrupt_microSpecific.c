#include "hal.h"
#include "hal_interrupt.h"

#include "hal_interrupt_microSpecific.h"
#include "stm32l031xx.h"
#include "stm32l0xx.h"
#include "system_stm32l0xx.h"

#include "shared_defs.h"

#define IMU_PIN 8

static hal_error_E hal_interrupt_microSpecific_initIMUChannel(void);

static const hal_interrupt_channelConfig_S 
    hal_interrupt_channelConfigs[HAL_INTERRUPT_CHANNEL_COUNT] = {

    [HAL_INTERRUPT_CHANNEL_IMU] =
        {
            .initChannel = hal_interrupt_microSpecific_initIMUChannel,
        },
};

static hal_interrupt_config_S hal_interrupt_config = {
    .channels   =   hal_interrupt_channelConfigs,
    .channelCount = HAL_INTERRUPT_CHANNEL_COUNT,
};

hal_error_E hal_interrupt_microSpecific_init(void) {
    return hal_interrupt_init(&hal_interrupt_config);
}

static hal_error_E hal_interrupt_microSpecific_initIMUChannel(void) {


    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

    // Set the interrupt pin as an input
    GPIOA->MODER &= ~(1 << IMU_PIN);

    // Enable the external interrupt on Port A pin 8
    SYSCFG->EXTICR[2] &= ~SYSCFG_EXTICR3_EXTI8_PA;

    //Remove the mask on line 8
    EXTI->IMR |= EXTI_IMR_IM8_Msk;

    //Ennable trigger on rising edge
    EXTI->RTSR |= EXTI_RTSR_RT8_Msk;

    //Enable the IRQ
    NVIC_EnableIRQ(EXTI4_15_IRQn);

    //Set to lowest priority
    NVIC_SetPriority(EXTI4_15_IRQn, 0);

    return HAL_ERROR_OK;

}