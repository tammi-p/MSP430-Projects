#include <msp430.h>
#include <generate_random.h>
#include <sounds.h>

unsigned int i;
unsigned int k;
unsigned int mpress;
unsigned int m;
unsigned int button;
unsigned int pressed;
unsigned int state = 0;
unsigned int turn;
unsigned int new_seed = 1;
int direction;
int first_time;
unsigned char displayseq[] = {0x00, 0x00, 0x00, 0x00,   // Start Frame
                                            0xE1, 0xFF, 0xFF, 0x00,   // LED Frame: 111Global BLUE GREEN RED
                                            0xE1, 0xFF, 0xFF, 0x00,
                                            0xE1, 0xFF, 0xFF, 0x00,
                                            0xE1, 0xFF, 0xFF, 0x00,
                                            0xFF, 0xFF, 0xFF, 0xFF }; // Last LED Frame

// Timeout
unsigned int t = 0;
unsigned int t_count;
int timeout;

// Gameover and Win sequences
const unsigned int gameover[] = {a3,c4,e4,g4Sh,a4,0}; // 0 is rest
const unsigned int gameover_len = sizeof(gameover)/sizeof(gameover[0]);
const unsigned int gameover_dur[] = {4,4,4,4,4};

const unsigned int win[] = {c4,e4,c4,e4,g4,0};
const unsigned int win_len = sizeof(win)/sizeof(win[0]);
const unsigned int win_dur[] = {4,4,4,4,4};

const long speed = 3500;

// Adjust sequence size
static const int M = 4; // sequence length
unsigned int sequence[M];
unsigned int rand_number;

void random_sequence()
{
    for (m=0; m<M; m++){
       rand_number = rand_gen();
       if (rand_number >= 0 && rand_number<= 16384){
           sequence[m] = 0x01;
       }
       else if (rand_number >= 16384 && rand_number <= 32768){
           sequence[m] = 0x02;
       }
       else if (rand_number > 32768 && rand_number <= 49152){
           sequence[m] = 0x03;
       }
       else{
           sequence[m] = 0x04;
       }
    }
}

void simons_turn()
{
    for(mpress=0; mpress<= turn; mpress++){
        button_press(sequence[mpress]);
        delay_ms(600);
        button_press(5);
        delay_ms(300);
    }
}

int main(void)
{

    BCSCTL3 |= LFXT1S_2;                      // ACLK = VLO
    WDTCTL = WDT_ADLY_1_9;                    // WDT 16ms, ACLK, interval timer
    IE1 = WDTIE;                              // Enable WDT interrupt

    // Initialize Piezo buzzer
    P2DIR |= BIT1;
    P2DIR |= BIT5;
    P2SEL |= BIT1;

    // Initialize Buttons
    P2DIR &= ~(BIT0 + BIT2 + BIT3 + BIT4);        // Inputs
    P2REN |= BIT0 + BIT2 + BIT3 + BIT4;           // Enable internal pull-up/down resistors
    P2OUT |= BIT0 + BIT2 + BIT3 + BIT4;           // Select pull-up mode (ensures voltage of Vcc across remainder of circuit when buttons aren't pressed)
    P2IE |= BIT0 + BIT2 + BIT3 + BIT4;            // Enable interrupt
    P2IES |= BIT0 + BIT2 + BIT3 + BIT4;           // IFG set on high to low edge transition
    P2IFG &= ~(BIT0 + BIT2 + BIT3 + BIT4);        // IFG cleared

    // Initialize LED
    P1DIR |= BIT2 + BIT4;                     // Set P1.2, 1.4 as output
    P1SEL = BIT2 + BIT4;                      // P1.2, 1.4 option select
    P1SEL2 = BIT2 + BIT4;

    UCA0CTL0 |= UCMST+UCSYNC+UCMSB;           // 8-bit SPI mstr, MSb 1st, CPOL=0, CPHS=0
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR1 = 0;
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

    // Set up temperature sensor (used in generating random number)
    ADC10CTL0 = SREF_1 + REFON + ADC10ON + ADC10SHT_3 ;     // 1.5V ref, Ref on, ADC10 on, sample for 64 cycles
    ADC10CTL1 = INCH_0+ ADC10DIV_3;                        // temp sensor is at 0 and clock/4

    __bis_SR_register(GIE);                   // Enable interrupts

    // state 0 - gameover seq played until player presses button to start game
    // state 1 - simon's turn and my turn
    // state 2 - win sequence played until player presses button to start game

    while(1){
        if(state == 0){
            if(new_seed){
                srand_seed();
                new_seed = 0;
            }
            random_sequence();
            for(i = 0; i < gameover_len; i++){
                PlaySound(gameover[i], gameover_len, (speed/gameover_dur[i]));
                gameover_led(i, (speed/gameover_dur[i]));
                delay_ms(100);
                if (state == 1){
                    pressed = 0;
                    button_press(5);
                    turn = 0;
                    break;
                }
            }
        }

        else if(state == 1){
            delay_ms(600);
            simons_turn();

            for(mpress = 0; mpress <= turn; mpress++){

                t_count = 1;
                if(turn <= 6){
                    timeout = -50*turn + 400;
                }
                while(t_count && t <= timeout){
                    __bis_SR_register(LPM0_bits);   // Enter LPM0 w/interrupt
                    if(pressed){
                        break;
                    }
                }
                t = 0;
                t_count = 0;

                if(pressed == 0 || button != sequence[mpress]){ // if timed out or pressed wrong button, state = 0
                    pressed = 0;
                    state = 0;
                    break;
                }
                else if(mpress == turn && turn == M-1){         // if played entire sequence correct, state = 2
                    pressed = 0;
                    state = 2;
                    first_time = 1;
                }
                else{                                           // pressed correct button
                    pressed = 0;
                }
            }
            if(turn < M-1){
                turn = turn+1;
            }
        }

        else if(state == 2){
            if(first_time){
                random_sequence();                          
                direction = 1;
                first_time = 0;
            }

            for(i = 0; i < win_len; i++){
                PlaySound(win[i], win_len, (speed/win_dur[i]));
                //win_led(i, (speed/win_dur[i]));

                __bis_SR_register(LPM0_bits);
                IE2 &= ~UCA0TXIE;

                // direction 1: Blue decrease, Green high, Red low
                // 0xFF (decimal 255) is a multiple of 0x11 (decimal 17)
                if(direction == 1){
                    if(displayseq[21] == 0x00){
                        direction = 2;
                    }
                    else{
                        displayseq[21] = displayseq[21] - 0x55;
                    }
                }
                //direction 2: Blue low, Green high, Red increase
                else if(direction == 2){
                    if(displayseq[23] == 0xFF){
                        direction = 3;
                    }
                    else{
                        displayseq[23] = displayseq[23] + 0x55;
                    }
                }
                //direction 3: Blue low, Green decrease, Red high
                else if(direction == 3){
                    if(displayseq[22] == 0x00){
                        direction = 4;
                    }
                    else{
                        displayseq[22] = displayseq[22] - 0x55;
                    }
                }
                //direction 4: Blue increase, Green low, Red high
                else if(direction == 4){
                    if(displayseq[21] == 0xFF){
                        direction = 5;
                    }
                    else{
                        displayseq[21] = displayseq[21] + 0x55;
                    }
                }
                //direction 5: Blue high, Green low, Red decrease
                else if(direction == 5){
                    if(displayseq[23] == 0x00){
                        direction = 6;
                    }
                    else{
                        displayseq[23] = displayseq[23] - 0x55;
                    }
                }
                //direction 6: Blue high, Green increase, Red low
                else if(direction == 6){
                    if(displayseq[22] == 0xFF){
                        direction = 1;
                    }
                    else{
                        displayseq[22] = displayseq[22] + 0x55;
                    }
                }

                displayseq[5] = displayseq[9];
                displayseq[6] = displayseq[10];
                displayseq[7] = displayseq[11];
                displayseq[9] = displayseq[13];
                displayseq[10] = displayseq[14];
                displayseq[11] = displayseq[15];
                displayseq[13] = displayseq[17];
                displayseq[14] = displayseq[18];
                displayseq[15] = displayseq[19];
                displayseq[17] = displayseq[21];
                displayseq[18] = displayseq[22];
                displayseq[19] = displayseq[23];

                //disable SPI interrupt
                for(k = 0; k < 28; k++){
                    //enable SPI interrupt and WDT
                    IE2 |= UCA0TXIE;
                    __bis_SR_register(LPM0_bits);
                    //interrupt
                    //load
                    //return
                    UCA0TXBUF = displayseq[k];
                }

                delay_ms(100);
                if (state == 1){
                    pressed = 0;
                    button_press(5);
                    turn = 0;
                    break;
                }
            }
        }
    }
}


// Timer A1 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A1_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
  __bic_SR_register_on_exit(LPM0_bits);  // Exit LMP0 when the interrupt returns
}

// SPI interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCIA0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCI0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if (IFG2&UCA0TXIFG){
    IFG2 &= ~UCA0TXIFG;
    __bic_SR_register_on_exit(LPM0_bits);
    }
}

// Watchdog Timer interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=WDT_VECTOR
__interrupt void watchdogi_timer(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void)
#else
#error Compiler not supported!
#endif
{
    if(t_count){
        t = t+1;
    }
    __bic_SR_register_on_exit(LPM0_bits);
}

// Port 2 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt void Port_2 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT2_VECTOR))) Port_2 (void)
#else
#error Compiler not supported!
#endif
{
  P2IE  &= ~(BIT0 + BIT2 + BIT3 + BIT4);   // Disable interrupt
  if ((state == 0 || state == 2) && (P2IFG&BIT0 || P2IFG&BIT2 || P2IFG&BIT3 || P2IFG&BIT4)){
      state = 1;
  }
  else if (state == 1){
      if (P2IFG&BIT0){
          button = 1;
      }
      else if (P2IFG&BIT2){
          button = 2;
      }
      else if (P2IFG&BIT3){
          button = 3;
      }
      else if (P2IFG&BIT4){
          button = 4;
      }

      button_press(button);
      pressed = 1;

      if(button == 1){
          while((P2IN & BIT0) == 0x00){}
      }
      else if(button == 2){
          while((P2IN & BIT2) == 0x00){}
      }
      else if(button == 3){
          while((P2IN & BIT3) == 0x00){}
      }
      else if(button == 4){
          while((P2IN & BIT4) == 0x00){}
      }
      button_press(5);
  }

  P2IFG &= ~(BIT0 + BIT2 + BIT3 + BIT4);   // IFG cleared
  P2IE  |= BIT0 + BIT2 + BIT3 + BIT4;      // Enable interrupt
  __bic_SR_register_on_exit(LPM0_bits);
}

#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    while(ADC10CTL1 & BUSY);
    __bic_SR_register(LPM0_bits);        // Return to active mode
}
