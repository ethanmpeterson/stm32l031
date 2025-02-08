// File to place interrupt handler implementation for this micro
// Names of each handler are found in the assembly startup code

// Standard Library Imports
#include <string.h>

// IO layer imports
#include "FreeRTOS.h"
#include "projdefs.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"

#include "stm32l0xx.h"
#include "system_stm32l0xx.h"

#include "hal.h"
#include "hal_uart.h"


// Device Layer Imports
#include "dev_console.h"
#include "dev_wifi.h"

extern xTaskHandle consoleTaskHandle;

extern xTaskHandle ESPTaskHandle;

volatile char receivedString[DEV_CONSOLE_MAX_COMMAND_LENGTH] = {0};
volatile uint32_t receivedCharCount = 0;
volatile bool receivedStringReady = false;

volatile uint8_t receivedESPCommand[DEV_WIFI_MAX_COMMAND_LENGTH] = {0};
extern volatile uint8_t receivedByteCount;
volatile bool receivedESPCommandReady = false;

void interrupts_init() {
  // ENABLE USART2 Interrupt in the NVIC
  // Use lowest priority, might need to tinker with this later
  NVIC_SetPriority(USART2_IRQn, 0x03);
  NVIC_EnableIRQ(USART2_IRQn);

  // ENABLE LPUART1 Interrupt in the NVIC
  // Use lowest priority
  receivedByteCount = 0;
  NVIC_SetPriority(LPUART1_IRQn, 0x03);
  NVIC_EnableIRQ(LPUART1_IRQn);
}

void USART2_IRQHandler(void) {
  // This ISR handler belongs to the COM_PORT / console UART.
  // Right now only the receive interrupt is enabled.

  // Clear the interrupt by reading the character
  char receivedChar;
  hal_uart_receiveChar(HAL_UART_CHANNEL_COM_PORT, &receivedChar);

  if (receivedStringReady) {
    // If the console task has not processed the previous command, discard the character
    return;
  }

  if (receivedCharCount < DEV_CONSOLE_MAX_COMMAND_LENGTH - 1) {
    // Put the char in the buffer
    receivedString[receivedCharCount] = receivedChar;
    receivedCharCount++;

    // If the received character is a new line, we know we have a complete
    // command and notify the console task.
    if (receivedChar == '\n') {
      // Indicate that the string is ready and then notify the console task
      receivedStringReady = true;
      vTaskNotifyGiveFromISR(consoleTaskHandle, NULL);
      receivedCharCount = 0;
    }
  } else {
    // If the buffer is full and we never received a newline then clear it
    memset((char *)receivedString, 0U, sizeof(receivedString));
    receivedCharCount = 0;
  }

}

void LPUART1_IRQHandler(void) {
  // This ISR handler belongs to the ESP_PORT / ESP LPUART
  // Right now only the receive interrupt is enabled.

  uint8_t receivedByte;
  hal_uart_receiveByte(HAL_UART_CHANNEL_ESP_PORT, &receivedByte);

 
  if (receivedESPCommandReady) {
    // Should think of a better way to deal with this as compared to the console there is the possibility to receive commands more quickly in succession
    return;
  }

  if (receivedByteCount < DEV_WIFI_MAX_COMMAND_LENGTH - 1) {
    // Put the byte in the buffer
    receivedESPCommand[receivedByteCount] = receivedByte;
    receivedByteCount++;

    if (receivedByte == DEV_WIFI_COMMAND_END) {
      // Indicate that the command has been received and then notify the wifi task
      receivedESPCommandReady = true;
      vTaskNotifyGiveFromISR(ESPTaskHandle, NULL);
    }
  } else {
    // If the buffer is full and we never received a command end then clear it
    memset((uint8_t *)receivedESPCommand, 0, sizeof(receivedESPCommand));
    receivedByteCount = 0;
  }


}