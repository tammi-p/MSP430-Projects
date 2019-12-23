#include <msp430.h>

#define CLOCKWISE 0
#define COUNTERCLOCKWISE 1

/* -------------------------------------------- *
   next_led(int direction)
Arguments:
    - direction - CLOCKWISE or COUNTERCLOCKWISE
Returns: the next LED in sequence in the given direction
    - unsigned int - lower byte maps to Port 2 pins
        and upper byte maps to Port 1 pins.
 * -------------------------------------------- */

unsigned int next_led(int direction)
{
  static unsigned int led = 0x00;

  // CODE
  // CW
  if (direction == CLOCKWISE)
  {
      if (led == 0x0000) // all leds off
      {
          led = 0x0001; // turn led1 on
      }
      else if (led == 0x0001) // led1 on
      {
          led = 0x8000; // turn led8 on
      }
      else if (led == 0x4000) // led7 on
      {
          led = 0x0020; // turn led6 on
      }
      else
      {
          led >>= 1;
      }
  }
  else // CCW
  {
      if (led == 0x0000)
      {
          led = 0x8000;
      }
      else if (led == 0x0020) // led6 on
      {
          led = 0x4000; // turn led7 on
      }
      else if (led == 0x8000) // led8 on
      {
          led = 0x0001; // turn led1 on
      }
      else
      {
          led <<= 1;
      }
  }
  return led;
}

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL3 = LFXT1S_2;                       // setting timer and value
  BCSCTL1 = CALBC1_1MHZ;                    // Set range
  DCOCTL = CALDCO_1MHZ;                     // Set DCO step + modulation */
  BCSCTL2 |= DIVS_3;                        // Divide MCLK by 8

  CCTL0 = CCIE;                             // CCRO interrupt enabled
  //CCR0 = 0x1458;
  CCR0 = 500;                               // when interrupt is called, timer counts up to 500
  //TACTL = TASSEL_2 + MC_1 + ID_3;         // SMCLK/8, upmode
  TACTL = TASSEL_1 + MC_1 + ID_3;           // ACLK/8, upmode


  // Set output pins
  P1DIR = 0xC0;                            // P1.6 and P1.7 are set to high (outputs)
  P2DIR = 0x3F;                            // P2.0 to 5 are set to high (outputs)
  P1OUT = 0;
  P2OUT = 0;


  // Configure pin interrupt
  P1IE  |= BIT2;     // P1.2 (connected to button) interrupt enabled
  P1IES |= BIT2;     // P1.2 Hi/lo edge
  P1IFG &= ~BIT2;    // P1.2 IFG (interrupt flag) cleared


  int value;
  int p1, p2;
  int direction = CLOCKWISE;
  int i;

  while(1) {
    for (i=0; i < 8*3; i++) {             // loops 24 times, lighting up 3 cycles of all 8 leds
      __bis_SR_register(LPM3_bits + GIE); // Enter LPM3 w/ Interrupt (Timer A interrupt)

      P1OUT &= ~0xC0;                     // P1.6 and P1.7 are set to low (inputs)
      P2OUT &= ~0x3F;                     // P2.0 to 5 are set to low (inputs)
      value = next_led(direction);
      p1 = (value & 0xFF00) >> 8;         // Port 1 pins that are set to high (corresponding LEDs are on)
                                          // Shift right by 8 because P1OUT has 8 bits, so it takes the value of the lower 8 bits of p1 later
      p2 = (value & 0x00FF);              // Port 2 pins that are set to high (corresponding LEDs are on)
      P1OUT |= p1;
      P2OUT |= p2;

    }
    if (direction == CLOCKWISE)
      direction = COUNTERCLOCKWISE;
    else direction = CLOCKWISE;
  }
}

// Timer A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
  __bic_SR_register_on_exit(LPM3_bits);  // Exit Low Power Mode 3
}

int asleep = 0;

// Port 1 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT1_VECTOR
__interrupt void Port_1 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) Port_1 (void)
#else
#error Compiler not supported!
#endif
{
  P1IE  &= ~BIT2;   // interrupt disabled for all pins except P1.2
  P1IFG &= ~BIT2;   // P1.2 IFG (interrupt flag) cleared (what and why?)
  if (asleep) {
    asleep = 0;
    P1IE  |= BIT2;
    __bic_SR_register_on_exit(LPM4_bits);  // Exit Low Power Mode 4
  }
  else {
    P1OUT &= ~0xC0;  // P1.6 and P1.7 are set to low (inputs)
    P2OUT &= ~0x3F;  // P2.0 to 5 pins are set to low (inputs)
    asleep = 1;
    P1IE  |= BIT2;
    __bis_SR_register_on_exit(LPM4_bits + GIE); // Enter Low Power Mode 4 w/ interrupts
  }
}
