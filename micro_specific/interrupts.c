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

extern xTaskHandle consoleTaskHandle;

volatile char receivedString[DEV_CONSOLE_MAX_COMMAND_LENGTH] = {0};
volatile uint32_t receivedCharCount = 0;
volatile bool receivedStringReady = false;

void interrupts_init() {
  // ENABLE USART2 Interrupt in the NVIC
  // Use lowest priority, might need to tinker with this later
  NVIC_SetPriority(USART2_IRQn, 0x03);
  NVIC_EnableIRQ(USART2_IRQn);
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
