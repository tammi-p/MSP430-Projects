/*
 * sounds.h
 *
 *  Created on: Mar 26, 2562 BE
 *      Author: tammitaphongmekhin
 */

#ifndef SOUNDS_H_
#define SOUNDS_H_

// Notes
#define e3 165
#define a3 220
#define c4 261
#define c4Sh 277
#define d4 293
#define d4Sh 311
#define e4 329
#define f4 349
#define f4Sh 370
#define g4 392
#define g4Sh 415
#define a4 440
#define a4Sh 466;
#define b4 493
#define c5 523
#define d5 587

#define clk_freq 1000000L

void delay_ms(unsigned int ms);
void PlaySound(int SoundString, int StringLength, int ToneDuration);
void animation(const unsigned int Song);

void button_press(int button);
void gameover_led(int idx, int ToneDuration);
void win_led(int idx, int ToneDuration);
void win_led2();

#endif /* SOUNDS_H_ */
