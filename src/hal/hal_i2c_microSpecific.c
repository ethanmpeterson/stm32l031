#include "hal.h"
#include "hal_i2c.h"

#include "hal_i2c_microSpecific.h"
#include "stm32l031xx.h"
#include "stm32l0xx.h"
#include "system_stm32l0xx.h"

#include "shared_defs.h"


hal_error_E hal_i2c_microSpecific_init(void) {

    //This is to configure it in master mode

    //clear PE bit
    I2C1->CR1 &= ~(1<<I2C_CR1_PE_Pos);

    //Enable analog filter
    I2C1->CR1 &= ~(1<<I2C_CR1_ANFOFF_Pos);

    //Disable digital filter
    I2C1->CR1 &= ~(I2C_CR1_DNF_Msk);

    //Configure timings
    //Generated with CubeMX might not be best practice
    I2C1->TIMINGR = 0x00B07CB4;

    //NOSTRETCH must be kept clear in master mode
    I2C1->CR1 &= ~(1<<I2C_CR1_NOSTRETCH_Pos);

    //Enable the peripheral
    I2C1->CR1 |= 1 << I2C_CR1_PE_Pos;
}

hal_error_E hal_i2c_sendData(uint8_t* data, uint8_t nBytes) {

    //Set number of bytes to be received
    I2C1->CR2 &= nBytes<<I2C_CR2_NBYTES_Pos;

    for (uint8_t i = 0; i < nBytes; i++) {

        //Wait until TXDR is empty
        while (!(I2C1->ISR & I2C_ISR_TXE_Msk));

        //Load next byte into TXDR
        I2C1->TXDR = *data;
        data++;
    }
}

hal_error_E hal_i2c_receiveData(uint8_t* data, uint8_t nBytes) {

    //Set number of bytes to be received
    I2C1->CR2 &= nBytes<<I2C_CR2_NBYTES_Pos;

    for (uint8_t i = 0; i < nBytes; i++) {
        while (!(I2C1->ISR & I2C_ISR_RXNE_Msk));
        *data = (uint8_t)I2C1->RXDR;

        data++;
    }
}

//This works with 7-bit address. For a 10 bit address either make a seperate function or handle both here
hal_error_E hal_i2c_setAddr(uint8_t addr) {

    //Clear the address bits
    I2C1->CR2 &= ~I2C_CR2_SADD_Msk;

    //Set the address
    I2C1->CR2 &= (I2C_CR2_SADD & addr)<<I2C_CR2_SADD_Pos;

    I2C1->CR2 &= I2C_CR2_START_Msk;

}

