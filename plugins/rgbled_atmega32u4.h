#pragma once

/*

*********************************
* Created 23 November 2013      *
* Copyright 2013, Brian Neltner *
* http://saikoled.com           *
* Licensed under GPL3           *
*********************************

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

A copy of the GNU General Public License can be found at
http://www.gnu.org/licenses/gpl.html

This software implements 4x 16-bit PWM with a fully saturated and
color corrected fading algorithm in HSI color space, and
HSI -> RGBW conversion to allow for better pastel color if desired.

It also implements an parabolic mapping for LED desired
brightness to actual output RGB value.

*/

#include <Arduino.h>

#define RED OCR3A
#define GREEN OCR1A
#define BLUE OCR1B

#define init_pwm init_pwm_atmega32u4

void init_pwm_atmega32u4() {
	// This section of setup ignores Arduino conventions because
	// they are too confusing to follow. Instead, I am figuring
	// this out directly from the datasheet for the ATmega32u4.

	// Pin Setup
	// Change the data direction on B5, B6, and B7 to
	// output without overwriting the directions of B0-4.
	// B5/OC1A is WHITE, B6/OC1B is BLUE, and B7/OC1C is GREEN.
	// These correspond to pins 9, 10, and 11 on an Arduino Leonardo.

	DDRB |= (1 << 5) | (1 << 6); //|(1<<7);

	// PWM Setup Stuff
	// TCCR1A is the Timer/Counter 1 Control Register A.
	// TCCR1A[7:6] = COM1A[1:0] (Compare Output Mode for Channel A)
	// TCCR1A[5:4] = COM1B[1:0] (Compare Output Mode for Channel B)
	// TCCR1A[3:2] = COM1C[1:0] (Compare Output Mode for Channel C)
	// COM1x[1:0] = 0b10 (Clear OC1x on compare match. Set at TOP)
	// TCCR1A[1:0] = WGM1[1:0] (Waveform Generation Mode for Timer 1 LSB)
	// TCCR1B[4:3] = WGM1[3:2] (Waveform Generation Mode for Timer 1 MSB)
	// WGM[3:0] = 0b1110 (Mode 14)
	// Mode 14 - Fast PWM, TOP=ICR1, Update OCR1x at TOP, TOVn Flag on TOP
	// So, ICR1 is the source for TOP.

	// Clock Stuff
	// TCCR1B[2:0] = CS1[2:0] (Clock Select for Timer 1)
	// CS1[2:0] = 001 (No Prescaler)

	// Unimportant Stuff
	// TCCR1B7 = ICNC1 (Input Capture Noise Canceler - Disable with 0)
	// TCCR1B6 = ICES1 (Input Capture Edge Select - Disable with 0)
	// TCCR1B5 = Reserved

	// And put it all together and... tada!
	TCCR1A = 0b10101010;
	TCCR1B = 0b00011001;

	// Writes 0xFFFF to the ICR1 register. THIS IS THE SOURCE FOR TOP.
	// This defines our maximum count before resetting the output pins
	// and resetting the timer. 0xFFFF means 16-bit resolution.

	ICR1 = 0xFFFF;

	// Similarly for Red, set the OC3A/PC6 pin to output.
	// This corresponds to Pin 5 on an Arduino Leonardo.

	DDRC |= (1 << 6);

	// In this case, TCCR3A is our control setup register, and it matches
	// TCCR1A in layout. We're only using Channel A (the others aren't
	// accessible anyway), so we can leave Channel B and C to 00. The
	// logic of WGM3[3:0] is identical, so we can leave that unchanged.
	// Similarly, TCCR3B has the same logic, so can be left unchanged.

	TCCR3A = 0b10000010;
	TCCR3B = 0b00011001;

	// And finally, we need to set ICR3 as the source for TOP in the Fast PWM.

	ICR3 = 0xFFFF;

	// Example of setting 16-bit PWM value.

	// Initial RGB values.
	RED = 0x0000;
	GREEN = 0x0000;
	BLUE = 0x0000;
}