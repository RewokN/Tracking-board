#ifndef TIMER_H_
#define TIMER_H_

#include "msp430g2553.h"
#include "config.h"

#define enable_timer()  (TA1CTL |= MC_1)    // Enable count up to CCR0
#define disable_timer() (TA1CTL &= ~MC_3)   // Halt the timer

/* timers_init()
 * SMCLK/8 UPmode
 * Delay: 2 ms
 * INPUT:  None
 * OUTPUT: None
 */
void timers_init(void);

/* timers_A0_interrupt();
 * Interrupt function
 * Delay:  1s
 * INPUT:  None
 * OUTPUT: None
 */
void timers_A0_interrupt(void);

#endif /* TIMER_H_ */
