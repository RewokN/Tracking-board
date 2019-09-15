#include <timer.h>
#include <uart.h>

volatile uint8 tx_flag;              // Mailbox Flag for the tx_char.
volatile uint8 tx_char;              // This char is the most current char to go into the UART
volatile uint8 rx_flag;              // Mailbox Flag for the rx_char.
volatile uint8 rx_char;              // This char is the most current char to come out of the UART

void uart_init(void)
{
    P1DIR &= ~ (RXD + TXD);
    P1DIR |= TXD;
    P1DIR &= ~RXD;
    P1SEL = RXD + TXD;               // Setup the I/O
    P1SEL2 = RXD + TXD;

    UCA0CTL1 |= UCSSEL_2;            // SMCLK
    UCA0BR0 = 104;                   // 104 From datasheet table-
    UCA0BR1 = 0;                     // -selects baudrate =9600,clk = SMCLK
    UCA0MCTL = UCBRS_1;              // Modulation value = 1 from datasheet
    UCA0CTL1 &= ~UCSWRST;            // Clear UCSWRST to enable USCI_A0
    IE2 |= UCA0RXIE;                 // Enable USCI_A0 RX interrupt

    rx_flag = 0;                     // Set rx_flag to 0
    tx_flag = 0;                     // Set tx_flag to 0
    return;
}

uint8 uart_getc()                    // Waits for a valid char from the UART
{
    while (rx_flag == 0);            // Wait for rx_flag to be set
    rx_flag = 0;                     // ACK rx_flag

    return rx_char;
}

boolean readMessage(uint8* Array)
{
    uint8 retVal;
    disable_timer();     // Disable timer interrupts while reading from UART
    if (rx_flag !=0)     // Check RX Flag
    {
        uart_gets(Array);
        retVal = TRUE;
    }
    else
    {
        retVal = FALSE;
    }
    enable_timer();     // Enable timer interrupts after reading
    return retVal;
}

boolean sendMessage(uint8* Array)
{
    if (Array != NULL)
    {
        disable_timer();
        uart_puts(Array);
        enable_timer();
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}

void uart_gets(uint8* Array)
{
    uint8 i = 0;                // Counter
    boolean waitNext = FALSE;   // SMS flag
    while (1)
    {
        Array[i] = uart_getc(); // Fill the array with characters from UART
        if(strcmp((sint8*)Array,"+CMT:") == 0)      // Check if the message is a SMS
            waitNext = TRUE;                        // Sets the SMS flag

        if (Array[(i>0)?i-1u:i] == '\r' && Array[i] == '\n') // Check the if the message ends with <CR> and <LF>
        {
            if(waitNext == TRUE)                    // Check if its a message
            {
                waitNext = FALSE;                   // Clear the flag
                Array[(i>0)?i-1u:i] =',';           // Change the end of the message with ',' to be extracted next
                continue;                           // Skip the next instruction
            }
            Array[(i>0)?i-1u:i] = '\0'; // if(i>0) return i-1, else return Array[i]
            return;                     // skip i++;
        }
        i++;
    }
}

void uart_putc(uint8 c)
{
    tx_char = c;                        // Put the char into the tx_char
    IE2 |= UCA0TXIE;                    // Enable USCI_A0 TX interrupt
    while (tx_flag == 1);               // Have to wait for the TX buffer
    tx_flag = 1;                        // Reset the tx_flag
    return;
}

void uart_puts(uint8 *str)              // Sends a String to the UART.
{
    while (*str)                        // Copy the elements string into TX buffer
        uart_putc(*str++);
    uart_putc('\r');
    uart_putc('\n');                    // End string with \r\n
    return;
}

#pragma vector = USCIAB0TX_VECTOR       // UART TX USCI Interrupt
__interrupt void USCI0TX_ISR(void)
{
    P1OUT |= LED_GREEN;                 // Notify that we received a char by toggling LED
    UCA0TXBUF = tx_char;                // Copy char to the TX Buffer
    tx_flag = 0;                        // ACK the tx_flag
    P1OUT &= ~LED_GREEN;                // Close LED_GREEN
    IE2 &= ~UCA0TXIE;                   // Turn off the interrupt to save CPU
}

#pragma vector = USCIAB0RX_VECTOR       // UART RX USCI Interrupt. This triggers when the USCI receives a char.
__interrupt void USCI0RX_ISR(void)
{
    P1OUT |= LED_RED;                   // Notify that we received a char by toggling LED
    rx_char = UCA0RXBUF;                // Copy from RX buffer, in doing so we ACK the interrupt as well
    rx_flag = 1;                        // Set the rx_flag to 1
    P1OUT &= ~LED_RED;
}
