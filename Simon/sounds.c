/*
 * sounds.c
 *
 *  Created on: Mar 26, 2562 BE
 *      Author: tammitaphongmekhin
 */
#ifndef SOUNDS_C_
#define SOUNDS_C_

#include <sounds.h>
#include <msp430.h>

unsigned int j;
volatile unsigned int period;

void delay_ms(unsigned int ms )
{
    for (j = 0; j<= ms; j++){
        __delay_cycles(500); // Built-in function that suspends the execution for 500 cycles
    }
}

void PlaySound(int SoundString, int StringLength, int ToneDuration)
{
    period = clk_freq / SoundString;                  // Timer Period = clock frea / note freq

    CCTL1 |= CCIE;                                    // Enable interrupt
    TA1CTL = TASSEL_2 + MC_1 + TACLR;                 // SMCLK, upmode
    TA1CCTL1 |=  OUTMOD_7;                            // Select Output Mode 7
    TA1CCR0 = period;                                 // Set timer period
    TA1CCR1 = 0;                                      // Init to case where we have a rest

    __bis_SR_register(LPM0_bits + GIE);               // Go to LPM0
     if(SoundString != 0){                            // For case where we have a note
         TA1CCR1 = period/2;                          // Generate output on TA1.1 at 50% duty cycle
         P1OUT ^= BIT2;                               // Send Timer Output to pin
     }
         delay_ms(ToneDuration);                      // Hold note/rest for spec'd duration
}

#endif /* SOUNDS_C_ */
