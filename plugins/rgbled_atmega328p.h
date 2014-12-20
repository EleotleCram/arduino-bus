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

#define RED OCR2A
#define GREEN OCR1A
#define BLUE OCR1B

#define init_pwm init_pwm_atmega328p

void init_pwm_atmega328p() {
	// Set PB1/2/3 as outputs.
	DDRB |= _BV(DDB1) | _BV(DDB1) | _BV(DDB3);

	TCCR1A =
		// Connect OCR1A to PB1 and OCR1B to PB2
		_BV(COM1A1) | _BV(COM1B1) |
		// Fast PWM mode.
		_BV(WGM11);
	TCCR1B =
		// Fast PWM mode.
		_BV(WGM12) | _BV(WGM13) |
		// 1/64 clock prescaler (~250Hz)
		_BV(CS10) | _BV(CS11);

	// Set the counter value that corresponds to
	// full duty cycle. For 15-bit PWM use
	// 0x7fff, etc. A lower value for ICR1 will
	// allow a faster PWM frequency.
	ICR1 = 0xffff;

	TCCR2A =
		// Connect OCR2A to PB3
		_BV(COM2A0) |
		// Fast PWM
		_BV(WGM20) | _BV(WGM21);
	TCCR2B =
		// 1/64 clock prescaler (~250Hz)
		_BV(CS22) |
		// Fast PWM (together with WGM20 and WGM21
		_BV(WGM22);

//	OCRA = 0xff;

	// Use OCR1A and OCR1B to control the PWM
	// duty cycle. Duty cycle = OCR1A / ICR1.
	// OCR1A controls PWM on pin 9 (PORTB1).
	// OCR1B controls PWM on pin 10 (PORTB2).
	OCR1A = 0;
	OCR1B = 0;
	OCR2A = 0;
}
