// File to place interrupt handler implementation for this micro
// Names of each handler are found in the assembly startup code

// Standard Library Imports

// IO layer imports
#include "hal.h"
#include "hal_uart.h"

// Device Layer Imports
#include "dev_console.h"

/* void USART2_IRQHandler(void) { */
/*   // This ISR handler belongs to the COM_PORT / console UART. */
/*   // Right now only the receive interrupt is enabled. */
/*   char receivedChar; */
/*   hal_uart_receiveChar(HAL_UART_CHANNEL_COM_PORT, &receivedChar); */

/*   // Feed the character into the console application */
/*   dev_console_collectCharFromISR(receivedChar); */
/* } */
