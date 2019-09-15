#include <timer.h>

extern boolean oneSecondPassed;

/* 1MHz/8 = 125,000 Hz
 * Time: 1/125000 = 8 us
 * Desired time 2ms : 2 / 0.008 = 250 To be load in TACCR0
 */
void timer_init(void)
{
    TACTL |= TACLR;                 // Clear TAR
    TACTL = TASSEL_2 + MC_1 + ID_3; // SMCLK + CountUp Mode + Divider with 8
    CCTL0 = CCIE;                   // Enable Timer Interrupt
    TACCR0 = 250;                   // 2ms
}

//Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void timers_A0_interrupt(void)
{
    static int countTime = 0;
    countTime++;                   // Count variable
    if (countTime > 500)
    {
        oneSecondPassed = TRUE;    // Set flag variable
        countTime = 0;             // Reset timer
    }
    // Automatically resets the timer flag
}
