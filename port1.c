
#include <port1.h>
//variables

void port_init(void)
{
    P2DIR |= 0xFF;                          // All P2.x outputs
    P2OUT &= 0x00;                          // All P2.x reset

    P1DIR &= 0x00;                          // P1DIR reset
    P1OUT &= 0x00;                          // P1OUT reset
    P1DIR |= LED_RED + LED_GREEN;    // P1DIR output on P1.0 and P1.6

    P1DIR |= MUX_SWITCH_BIT;                // MUX_SWITCH_BIT output

    return;
}

/* MUX Table 74HC4053 2:1 MUX/DEMUX
 *
 *  Connection PINS
 *     GSM_TX -> 2Y1|   |VCC  <- 3v3
 *     GPS_TX -> 2Y0|   |2Z   <- RX_MCU
 *               3Y1|   |1Z   <- TX_MCU
 *                3Z|   |1Y1  <- RX_GSM
 *               3Y0|   |1Y0  <- RX_GPS
 *        GND ->  ~E|   |S1   <-|__MUX_SWITCH_BIT
 *               VEE|   |S2   <-|
 *        GND -> GND|   |S3
 *
 *     S1&S2| 1Z(TX)->1Y0,1Y1(RX) | 2Z(RX)<-2Y0,2Y1(TX) |
 *         0|        1Y0          |         2Y0         |
 *         1|        1Y1          |         2Y1         |
 *
 *      BIT4|tx_mcu->rx_gps,rx_gsm|rx_mcu<-tx_gps,tx_gsm|
 * TO_GPS(0)|       rx_gps        |       tx_gps        |
 * TO_GSM(1)|       rx_gsm        |       tx_gsm        |
 *
 * */
void mux_switch(stare_mux mux)
{
    if (mux == TO_GPS)
    {
        P1OUT &= ~MUX_SWITCH_BIT;  // switch from GSM(1) to GPS (0)
    }
    if (mux == TO_GSM)
    {
        P1OUT |= MUX_SWITCH_BIT;   // switch from GPS(0) to GSM (1)
    }
    return;
}
