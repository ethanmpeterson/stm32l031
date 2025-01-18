#include <stdint.h>

#include "stm32l031xx.h"
#include "stm32l0xx.h"

#include "system_stm32l0xx.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "hal.h"
#include "hal_uart.h"
#include "hal_uart_microSpecific.h"

#include "hal_gpio.h"
#include "hal_gpio_microSpecific.h"

#include "hal_rtc.h"
#include "hal_rtc_microSpecific.h"

#include "hal_i2c.h"
#include "hal_i2c_microSpecific.h"

#include "interrupts.h"

// Device / Board Layer Imports

// Application Layer Imports

#define UART_BAUD_RATE 115200

static void blinky_task(void *pvParameters) {

  TickType_t lastWakeTime = xTaskGetTickCount();
  // Blink the led at 10Hz
  const TickType_t freq = 100;

  for (;;) {
    hal_gpio_pinState_E currentPinState = hal_gpio_readInputState(HAL_GPIO_CHANNEL_LED);
    hal_gpio_pinState_E nextPinState;
    if (currentPinState == HAL_GPIO_PINSTATE_ON) {
      nextPinState = HAL_GPIO_PINSTATE_OFF;
    } else {
      nextPinState = HAL_GPIO_PINSTATE_ON;
    }
    hal_gpio_setOutputState(HAL_GPIO_CHANNEL_LED, nextPinState);

    vTaskDelayUntil(&lastWakeTime, freq);
  }
}

static void rtc_task(void *pvParameters) {
  TickType_t lastWakeTime = xTaskGetTickCount();
  const TickType_t freq = 1000;

  for (;;) {
    // Transfer the byte if the peripheral is available
    hal_rtc_time_S currentTime;
    currentTime.seconds = 0;
    (void)hal_rtc_getTime(&currentTime);
    char secondsTens = (char)((currentTime.seconds / 10) % 10) + '0';
    char secondsUnits = (char)(currentTime.seconds % 10) + '0';
    char minutesTens = (char)((currentTime.minute / 10) % 10) + '0';
    char minutesUnits = (char)(currentTime.minute % 10) + '0';
    char hoursTens = (char)((currentTime.hour / 10) % 10) + '0';
    char hoursUnits = (char)(currentTime.hour % 10) + '0';
    char dayTens = (char)((currentTime.day / 10) % 10) + '0';
    char dayUnits = (char)(currentTime.day % 10) + '0';
    char monthTens = (char)((currentTime.month / 10) % 10) + '0';
    char monthUnits = (char)(currentTime.month % 10) + '0';
    char yearTens = (char)((currentTime.year / 10) % 10) + '0';
    char yearUnits = (char)(currentTime.year % 10) + '0';
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, '2');
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, '0');
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, yearTens);
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, yearUnits);
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, '/');
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, monthTens);
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, monthUnits);
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, '/');
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, dayTens);
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, dayUnits);

    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, '-');

    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, hoursTens);
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, hoursUnits);
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, ':');
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, minutesTens);
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, minutesUnits);
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, ':');
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, secondsTens);
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, secondsUnits);
    (void)hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, '\n');

    vTaskDelayUntil(&lastWakeTime, freq);
  }
}

void USART2_IRQHandler(void) {
  char receivedChar;
  hal_uart_receiveChar(HAL_UART_CHANNEL_COM_PORT, &receivedChar);
  hal_uart_sendChar(HAL_UART_CHANNEL_COM_PORT, receivedChar);
}

//Perform IRQ handling here, read value and clear flag call from microspecific
void I2C1_IRQHandler(void) {
  if (I2C1->ISR & I2C_ISR_TC_Msk) {
    hal_i2c_data_transfer_complete(HAL_I2C_CHANNEL_IMU);
  }
  if (I2C1->ISR & I2C_ISR_TXIS_Msk) {
    //Call function to put next byte in I2C_TXDR
    hal_i2c_transmit_next_byte(HAL_I2C_CHANNEL_IMU);
  }
  if (I2C1->ISR & I2C_ISR_RXNE_Msk) {
    //Call function to store byte from I2C_RXDR
    hal_i2c_receive_next_byte(HAL_I2C_CHANNEL_IMU);
  }
}


int main(void) {
  (void)hal_init();

  (void)hal_rtc_microSpecific_init();
  (void)hal_gpio_microSpecific_init();
  (void)hal_uart_microSpecific_init();
  (void)hal_i2c_microSpecific_init();

  hal_rtc_time_S initialTime = {
    .year = 0, // 2000
    .month = 9, // September
    .day = 14,
    .weekday = 4,
    .hour = 13, // 1PM (24H format)
    .minute = 32,
    .seconds = 54
  };

  hal_rtc_setTime(&initialTime);

  // ENABLE USART2 Interrupt in the NVIC
  // Use lowest priority, might need to tinker with this later
  NVIC_SetPriority(USART2_IRQn, 0x03);
  NVIC_EnableIRQ(USART2_IRQn);


  //Enable I2C1 Interrupt in the NVIC using lowest priority
  NVIC_SetPriority(I2C1_IRQn, 0x03);
  NVIC_EnableIRQ(I2C1_IRQn);

  for (;;);

  // These handles are used to identify the tasks and reference them.
  // Suspend, resume, notify etc.
  xTaskHandle blinkyTaskHandle;
  xTaskHandle rtcTaskHandle;

  xTaskCreate(blinky_task, "blinky", 50, NULL, tskIDLE_PRIORITY, &blinkyTaskHandle);
  xTaskCreate(rtc_task, "rtc", 50, NULL, tskIDLE_PRIORITY + 1, &rtcTaskHandle);

  vTaskStartScheduler();

  return 0;
}
