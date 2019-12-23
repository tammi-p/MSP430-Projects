/*
 * lab9_header.h
 *
 *  Created on: Mar 17, 2562 BE
 *      Author: tammitaphongmekhin
 */

#ifndef GENERATE_RANDOM_H_
#define GENERATE_RANDOM_H_

# include <stdint.h>
// static unsigned int rand_seed;

    uint16_t lfsr; // = 0xACE1u;
    uint16_t bit;

/* Return a random integer between 0 and RAND_MAX (31) inclusive.*/
int rand_gen(){
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5));
    lfsr = (lfsr >> 1) | (bit << 15);
    return lfsr;
}

void srand_seed()
{
    ADC10CTL0 |= ENC + ADC10SC;
    __bis_SR_register(LPM0_bits);
    lfsr = ADC10MEM;
}

#endif /* GENERATE_RANDOM_H_ */
