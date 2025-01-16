#include "hal.h"
#include "hal_i2c.h"

#include "hal_i2c_microSpecific.h"
#include "stm32l031xx.h"
#include "stm32l0xx.h"
#include "system_stm32l0xx.h"

//all should be private because of the static
static hal_error_E hal_i2c_microSpecific_initI2CChannel1(void);
static hal_error_E hal_i2c_microSpecific_sendChannel1Data(uint16_t address, uint8_t* data, uint8_t nBytes);
static hal_error_E hal_i2c_receiveI2CChannel1Data(uint16_t address, uint8_t* data, uint8_t nBytes);

static const hal_i2c_channelConfig_S hal_i2c_channelConfigs[HAL_I2C_CHANNEL_COUNT] = {
    [HAL_I2C_CHANNEL_1] = {
        .initChannel = hal_i2c_microSpecific_initI2CChannel1,
        .sendData    = hal_i2c_microSpecific_sendChannel1Data
    }
};

static hal_i2c_config_S hal_i2c_config = {
    .channels     = hal_i2c_channelConfigs,
    .channelCount = HAL_I2C_CHANNEL_COUNT

};

hal_error_E hal_i2c_microSpecific_init(void) {

    return hal_i2c_init(&hal_i2c_config);
}

hal_error_E hal_i2c_microSpecific_initI2CChannel1(void) {

    //This is to configure it in master mode

    //clear PE bit
    I2C1->CR1 &= ~(1 << I2C_CR1_PE_Pos);

    //Enable analog filter
    I2C1->CR1 &= ~(1 << I2C_CR1_ANFOFF_Pos);

    //Disable digital filter
    I2C1->CR1 &= ~I2C_CR1_DNF_Msk;

    //Configure timings
    //Generated with CubeMX might not be best practice

    

    I2C1->TIMINGR = 0x00B07CB4;

    //NOSTRETCH must be kept clear in master mode
    I2C1->CR1 &= ~(1 << I2C_CR1_NOSTRETCH_Pos);

    //Enable the peripheral
    I2C1->CR1 |= (1 << I2C_CR1_PE_Pos);

    return HAL_ERROR_OK;
}


hal_error_E hal_i2c_microSpecific_sendChannel1Data(uint16_t address, uint8_t* data, uint8_t nBytes) {

    //Set number of bytes to be received

    uint16_t addressWidthMsk = 0xFC00;

    if (addressWidthMsk & address) {
        //Return error
    }

    //Clear the address bits
    I2C1->CR2 &= ~I2C_CR2_SADD_Msk;

    //Set the address
    I2C1->CR2 &= I2C_CR2_SADD & address << I2C_CR2_SADD_Pos;

    //Start
    I2C1->CR2 &= I2C_CR2_START_Msk;



    I2C1->CR2 &= nBytes << I2C_CR2_NBYTES_Pos;

    for (uint8_t i = 0; i < nBytes; i++) {

        //Wait until TXDR is empty
        while (!(I2C1->ISR & I2C_ISR_TXE_Msk));

        //Load next byte into TXDR
        I2C1->TXDR = *data;
        data++;
    }
}

hal_error_E hal_i2c_receiveI2CChannel1Data(uint16_t address, uint8_t* data, uint8_t nBytes) {
    
    uint16_t addressWidthMsk = 0xFC00;

    if (addressWidthMsk & address) {
        //Return error
    }

    //Clear the address bits
    I2C1->CR2 &= ~I2C_CR2_SADD_Msk;

    //Set the address
    I2C1->CR2 &= I2C_CR2_SADD & address << I2C_CR2_SADD_Pos;

    //Start
    I2C1->CR2 &= I2C_CR2_START_Msk;

    //Set number of bytes to be received
    I2C1->CR2 &= nBytes << I2C_CR2_NBYTES_Pos;

    for (uint8_t i = 0; i < nBytes; i++) {
        while (!(I2C1->ISR & I2C_ISR_RXNE_Msk));
        *data = (uint8_t)I2C1->RXDR;

        data++;
    }
}
