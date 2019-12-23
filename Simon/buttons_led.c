/*
 * buttons.c
 *
 *  Created on: Mar 26, 2562 BE
 *      Author: tammitaphongmekhin
 */
#ifndef BUTTONS_C_
#define BUTTONS_C_

#include <msp430.h>
#include <sounds.h>

unsigned int k;

void button_press(int button)
{
    unsigned char displayseq[] = {0x00, 0x00, 0x00, 0x00,   // Start Frame
                        0xE1, 0x00, 0x00, 0x00,   // LED Frame: 111Global BLUE GREEN RED
                        0xE1, 0x00, 0x00, 0x00,
                        0xE1, 0x00, 0x00, 0x00,
                        0xE1, 0x00, 0x00, 0x00,
                        0xFF, 0xFF, 0xFF, 0xFF}; // Last LED Frame
    IE2 &= ~UCA0TXIE; // Disable SPI interrupt

    if (button == 1){ // magenta
        displayseq[5] = 0xFF;
        displayseq[7] = 0xFF;
        PlaySound(a3,1,4);
    }
    else if (button == 2){ // light blue
        displayseq[9] = 0xFF;
        displayseq[10] = 0xA0;
        PlaySound(e4,1,4);
    }
    else if (button ==3){ // bright yellow
        displayseq[18] = 0xFF;
        displayseq[19] = 0xD0;
        PlaySound(c4Sh,1,4);
    }
    else if (button == 4){ // green
        displayseq[13] = 0x10;
        displayseq[14] = 0xFF;
        PlaySound(e3,1,4);
    }
    else{                       // No lights, no sound
        PlaySound(0,1,4);
    }

    for(k = 0; k < 24; k++){
        //enable SPI interrupt and WDT
        IE2 |= UCA0TXIE;
        //interrupt
        //load
        //return
        UCA0TXBUF = displayseq[k];
    }
}

void gameover_led(int idx, int ToneDuration){
    unsigned char displayseq[] = {0x00, 0x00, 0x00, 0x00,   // Start Frame
                        0xE1, 0x00, 0x00, 0x00,   // LED Frame: 111Global BLUE GREEN RED
                        0xE1, 0x00, 0x00, 0x00,
                        0xE1, 0x00, 0x00, 0x00,
                        0xE1, 0x00, 0x00, 0x00,
                        0xFF, 0xFF, 0xFF, 0xFF}; // Last LED Frame

    __bis_SR_register(LPM0_bits);
    IE2 &= ~UCA0TXIE;

    if (idx == 0 || idx == 2 || idx == 4){
        // led1 is purple
        displayseq[5] = 0xFF;
        displayseq[7] = 0x30;
        // led2 is white
        displayseq[9] = 0xFF;
        displayseq[10] = 0xFF;
        displayseq[11] = 0xFF;
        // led 3 is cream
        displayseq[17] = 0xA0;
        displayseq[18] = 0xA0;
        displayseq[19] = 0xFF;
        // led4 is magenta
        displayseq[13] = 0xFF;
        displayseq[15] = 0xFF;
    }
    else{
        // led1 is cream
        displayseq[5] = 0xA0;
        displayseq[6] = 0xA0;
        displayseq[7] = 0xFF;
        // led2 is magenta
        displayseq[9] = 0xFF;
        displayseq[11] = 0xFF;
        // led3 is purple
        displayseq[17] = 0xFF;
        displayseq[19] = 0x30;
        // led4 is white
        displayseq[13] = 0xFF;
        displayseq[14] = 0xFF;
        displayseq[15] = 0xFF;
    }

    for(k = 0; k < 24; k++){
        //enable SPI interrupt and WDT
        IE2 |= UCA0TXIE;
        __bis_SR_register(LPM0_bits);
        //interrupt
        //load
        //return
        UCA0TXBUF = displayseq[k];
    }
}


void win_led(int idx, int ToneDuration){
    unsigned char displayseq[] = {0x00, 0x00, 0x00, 0x00,   // Start Frame
                        0xE1, 0x00, 0x00, 0x00,   // LED Frame: 111Global BLUE GREEN RED
                        0xE1, 0x00, 0x00, 0x00,
                        0xE1, 0x00, 0x00, 0x00,
                        0xE1, 0x00, 0x00, 0x00,
                        0xFF, 0xFF, 0xFF, 0xFF}; // Last LED Frame

    __bis_SR_register(LPM0_bits);
    IE2 &= ~UCA0TXIE;

    if (idx == 0 || idx == 2 || idx == 4 || idx == 6){
        // led1 is yellow
        displayseq[6] = 0xFF;
        displayseq[7] = 0xFF;
        // led2 is white
        displayseq[9] = 0xFF;
        displayseq[10] = 0xFF;
        displayseq[11] = 0xFF;
        // led 3 is dark green
        displayseq[17] = 0x70;
        displayseq[18] = 0xFF;
        // led4 is light green
        displayseq[14] = 0xFF;
        displayseq[15] = 0x70;
    }
    else{
        // led1 is dark green
        displayseq[5] = 0x70;
        displayseq[6] = 0xFF;
        // led2 is light green
        displayseq[10] = 0xFF;
        displayseq[11] = 0x70;
        // led3 is yellow
        displayseq[18] = 0xFF;
        displayseq[19] = 0xFF;
        // led4 is white
        displayseq[13] = 0xFF;
        displayseq[14] = 0xFF;
        displayseq[15] = 0xFF;
    }

    for(k = 0; k < 28; k++){
        //enable SPI interrupt and WDT
        IE2 |= UCA0TXIE;
        __bis_SR_register(LPM0_bits);
        //interrupt
        //load
        //return
        UCA0TXBUF = displayseq[k];
        }
}


void win_led2(unsigned char displayseq[]){
    int direction = 1;

    __bis_SR_register(LPM0_bits);
    IE2 &= ~UCA0TXIE;

            //idx 1: Blue decrease, Green high, Red low
            // 0xFF (decimal 255) is a multiple of 0x11 (decimal 17)
            if(direction == 1){
                            if(displayseq[21] == 0x00){
                                direction = 2;
                            }
                            else{
                                displayseq[21] = displayseq[21] - 0x55;
                            }
                                                        }
            //idx 2: Blue low, Green high, Red increase
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
}

#endif /* BUTTONS_C_ */
