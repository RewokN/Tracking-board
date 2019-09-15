
#ifndef UART_H_
#define UART_H_

#include "config.h"

// variables
extern volatile uint8 rx_flag;

// functions
void uart_flag_init(void);

/*  uart_init()
 *  Sets up the UART interface via USCI
 *  UART baudrate: 9000 for 1Mhz SMCLK DCO
 *  Configuration: 8N1
 *  P1.1 RX
 *  P1.2 TX
 *  INPUT: None
 *  RETURN: None
 */
void uart_init(void);

/*uart_getc()
* Get a char from the UART. Waits till it gets one
* INPUT: None
* RETURN: Char from UART
*/
uint8 uart_getc(void);

/*uart_gets()
* Get a string from the UART. Strings terminate when it
* contains carriage return and line feed. Waits for the data.
* INPUT: Array pointer
* RETURN: None
*/
void uart_gets(uint8 *Array);

/*readMessage()
* Deactivates/Activates the timer while calling uart_gets
* INPUT: Array pointer
* RETURN: boolean retValue
*/
boolean readMessage(uint8* Array);

/*uart_putc()
* Sends a char to the UART. Will wait if the UART is busy
* INPUT: Char to send
* RETURN: None
*/
void uart_putc(uint8 c);

/*uart_puts()
* Sends a string to the UART. Will wait if the UART is busy
* INPUT: Pointer to String to send
* RETURN: None
*/
void uart_puts(uint8 *str);

/*sendMessage()
* Deactivates/Activates the timer while calling uart_puts
* INPUT: Array pointer
* RETURN: boolean retValue
*/
boolean sendMessage(uint8* Array);
#endif /* UART_H_ */
