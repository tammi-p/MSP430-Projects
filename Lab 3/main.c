/* Based on msp430g2xx3_wdt_02.c from the TI Examples */

#include <msp430.h>

int main(void)
{
    BCSCTL3 |= LFXT1S_2;                      // ACLK = VLO
    WDTCTL = WDT_ADLY_250;                    // WDT 250ms, ACLK, interval timer
    IE1 |= WDTIE;                             // Enable WDT interrupt
    P1DIR |= BIT6;                            // Set P1.6 (led 7) as output
    P1SEL |= BIT6;                            // Select PWM for P1.6
    P2DIR |= BIT2;                            // Set P2.2 (led 3) as output
    P2SEL |= BIT2;                            // Select PWM for P2.2

    TA0CCR0 = 80;                             // Set period
    TA0CCTL1 = OUTMOD_7;
    TA0CCR1 = 0;                              // 0% duty cycle

    TA1CCR0 = 80;                             // Set period
    TA1CCTL1 = OUTMOD_7;
    TA1CCR1 = 80;                             // 100% duty cycle

    TA0CTL = TASSEL_1 + MC_1;           // ACLK/8, upmode
    TA1CTL = TASSEL_1 + MC_1;           // ACLK/8, upmode

    __bis_SR_register(GIE);             // Enable interrupts
    while (1) {
        __bis_SR_register(LPM3_bits);   // Enter LPM3 w/interrupt
        P1OUT ^= BIT6;
        P2OUT ^= BIT2;
        static unsigned int  direction = 1;                 // 1 - led 7 brightens, 0 - led 3 brightens
        if (direction)                                      // Direction 1
             {
               TA0CCR1 += 4;                                 // Add/Subtract 5% to existing value
               TA1CCR1 -= 4;
               if (TA0CCR1 == 80)                            // Reached 100% / 0% duty cycle
               {
                 direction = 0;                              // Next time direction 0
               }
             }
         else                                               // Direction 0
             {
               TA0CCR1 -= 4;                                 // Subtract/Add 5% to existing value
               TA1CCR1 += 4;
               if (TA0CCR1 == 0)                             // Reached 0% / 100% duty cycle
               {
                 direction = 1;                              // Next time direction 1
               }
             }
     }

}


// Watchdog Timer interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void)
#else
#error Compiler not supported!
#endif
{
    __bic_SR_register_on_exit(LPM3_bits);
}

