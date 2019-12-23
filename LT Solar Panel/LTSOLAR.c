#include <msp430.h>

#define servo_freq 50
#define freq 1100000L
#define steps 180
#define servomin 540
#define servomax 2520

int i;
unsigned int degree;
unsigned int samp;
unsigned int samples[2];
volatile unsigned int period;
unsigned int value = 0;


unsigned int step_cal, step_temp;
unsigned int servo_st[steps+1];

void servo()
{
    period = freq / servo_freq;               // Calculate period
    step_cal = ((servomax - servomin) / steps); // Calculate the step size per degree (angle)
    step_temp = servomin;

    for (i = 0; i < steps; i++) {
        step_temp += step_cal;
        servo_st[i] = step_temp;
    }

    CCTL0 |= CCIE;                             // Enable CCR0 interrupt
    TA1CTL |= TASSEL_2 + MC_1 + TACLR;         // SMCLK, Up Mode, Clear
    TA1CCTL1 |=  OUTMOD_7;                     // Select Output Mode 7
    TA1CCR0 = period - 1;                      // Assign period
    TA1CCR1  = 0;                              // Assign 0

    P2OUT ^= BIT2;                             // Toggle P2.2
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    ADC10CTL1 = INCH_1 + CONSEQ_3; // input A0 and A1, Repeat sequence of channels
    ADC10CTL0 = SREF_0 + ADC10SHT_2 + ADC10ON + ADC10IE + REFON + MSC; // ADC10 enabled, interrupt enabled
    ADC10AE0 |= BIT0 + BIT1; // Enable A0 and A1
    ADC10DTC1 = 0x02; // Select 2 data transfers

    P2DIR |= BIT2;                            // Set P2.2 to output direction
    P2SEL |= BIT2;                            // Set P2.2 for PWM output
    P2SEL2 &= ~BIT2;                          // Select PWM output for P2.2
    servo();

    P1DIR |= BIT6;                            //Set 1.6 as output
    P1OUT &= ~BIT6;


    while (1) {
        ADC10CTL0 &= ~ENC;                      // Disable ADC10
        while (ADC10CTL1 & BUSY);               // Wait if ADC10 core is active
        ADC10SA = (unsigned int)samples;        // Assign two samples
        ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start

        __bis_SR_register(LPM0_bits + GIE);     // Enable LPM0 and global interrupts


        if (samples[0] < samples[1]){
            P1OUT |= BIT6; // Bluetooth signal
            samp = (samples[1] - samples[0])/4;
            if(samp > 90){ // Max angle at 179
                degree = 179;
            }
            else{ // 90 to 179 degrees
                degree = 90 + samp;
            }
            TA1CCR1 = servo_st[degree];
            __delay_cycles(2000);
        }
        else if(samples[0] > samples[1]){
            P1OUT &= ~BIT6;
            samp = (samples[0] - samples[1])/4;
            if(samp > 90){ // Min angle at 0
                degree = 0;
            }
            else{ // 1 to 90 degrees
                degree = 90 - samp;
            }
            TA1CCR1 = servo_st[degree];
            __delay_cycles(2000);
        }
        else{ // 90 degrees
            TA1CCR1 = servo_st[90];
        }

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
  __bic_SR_register_on_exit(LPM0_bits);  // Exit LMP0 when the interrupt returns
}


// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
__bic_SR_register_on_exit(LPM0_bits); // Exit LMP0 when the interrupt returns
}
