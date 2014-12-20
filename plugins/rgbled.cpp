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

#include "debug.h"
#include "loop.h"
#include <abus/ABus.h>
#include "rgbled.h"

// #include MCU_HEADER(x)   =>   #include "x_<MCU>.h"
#define MCU_HEADER(x) STR(JOIN_TOKENS(x,_,MCU.h))
// Token pasting magic (two-level macro is to allow for dereferencing since
// the # and ## operators will not dereference):
#define STR_(x) #x
#define STR(x) STR_(x)
#define JOIN_TOKENS(x,y,z) JOIN_TOKENS_(x,y,z)
#define JOIN_TOKENS_(x,y,z) x ## y ## z

#include MCU_HEADER(rgbled)

#include <math.h>
#define DEG_TO_RAD_FUNC(X) (M_PI*(X)/180)

#define steptime 1
#define saturation 0.3
#define hue_increment 0.01

int whitevalue;

struct HSI {
  float h;
  float s;
  float i;
} color;

void updatehue();
void updatesaturation();
void sendcolor();
void hsi2rgbw(float H, float S, float I, int* rgbw);

void init_plugin_rgbled()  {
  init_pwm();

  color.h = 0;
  color.s = saturation;
  color.i = 1;

  sendcolor();
/*
  // Initial color = off, hue of red fully saturated.
  while (color.i < 1) {
    sendcolor();
    color.i = color.i + 0.001; // Increase Intensity
    updatehue();
    updatesaturation();
    delay (steptime);
  }
*/
	ABus.subscribe([&] (const uint16_t msgId, const void* pArg) {
		// handle message
		switch(msgId) {
			case MSG_LOOP_TICK:
				sendcolor();
				updatehue();
				updatesaturation();
				break;
			case MSG_RGBLED_ON:
				LOG_INFO("MSG_RGBLED_ON");
				color.i = 1;
				sendcolor();
				break;
			case MSG_RGBLED_OFF:
				LOG_INFO("MSG_RGBLED_OFF");
				color.i = 0;
				sendcolor();
				break;
		}
	});
}

void updatehue() {
  color.h = color.h + hue_increment;
}

void updatesaturation() {
  color.s = saturation;
}

void sendcolor() {
  int rgb[3];
  while (color.h >=360) color.h = color.h - 360;
  while (color.h < 0) color.h = color.h + 360;
  if (color.i > 1) color.i = 1;
  if (color.i < 0) color.i = 0;
  if (color.s > 1) color.s = 1;
  if (color.s < 0) color.s = 0;
  // Fix ranges (somewhat redundantly).
  hsi2rgbw(color.h, color.s, color.i, rgb);
  RED = rgb[0];
  GREEN = rgb[1];
  BLUE = rgb[2];
}

void hsi2rgbw(float H, float S, float I, int* rgbw) {
  float r, g, b, w;
  float cos_h, cos_1047_h;
  H = fmod(H,360); // cycle H around to 0-360 degrees
  H = 3.14159*H/(float)180; // Convert to radians.
  S = S>0?(S<1?S:1):0; // clamp S and I to interval [0,1]
  I = I>0?(I<1?I:1):0;

  // This section is modified by the addition of white so that it assumes
  // fully saturated colors, and then scales with white to lower saturation.
  //
  // Next, scale appropriately the pure color by mixing with the white channel.
  // Saturation is defined as "the ratio of colorfulness to brightness" so we will
  // do this by a simple ratio wherein the color values are scaled down by (1-S)
  // while the white LED is placed at S.

  // This will maintain constant brightness because in HSI, R+B+G = I. Thus,
  // S*(R+B+G) = S*I. If we add to this (1-S)*I, where I is the total intensity,
  // the sum intensity stays constant while the ratio of colorfulness to brightness
  // goes down by S linearly relative to total Intensity, which is constant.

  if(H < 2.09439) {
    cos_h = cos(H);
    cos_1047_h = cos(1.047196667-H);
    r = S*I/3*(1+cos_h/cos_1047_h);
    g = S*I/3*(1+(1-cos_h/cos_1047_h));
    b = 0;
    w = (1-S)*I;
  } else if(H < 4.188787) {
    H = H - 2.09439;
    cos_h = cos(H);
    cos_1047_h = cos(1.047196667-H);
    g = S*I/3*(1+cos_h/cos_1047_h);
    b = S*I/3*(1+(1-cos_h/cos_1047_h));
    r = 0;
    w = (1-S)*I;
  } else {
    H = H - 4.188787;
    cos_h = cos(H);
    cos_1047_h = cos(1.047196667-H);
    b = S*I/3*(1+cos_h/cos_1047_h);
    r = S*I/3*(1+(1-cos_h/cos_1047_h));
    g = 0;
    w = (1-S)*I;
  }

  // Mapping Function from rgbw = [0:1] onto their respective ranges.
  // For standard use, this would be [0:1]->[0:0xFFFF] for instance.

  // Here instead I am going to try a parabolic map followed by scaling.
  rgbw[0]=0xFFFF*r*r;
  rgbw[1]=0xFFFF*g*g;
  rgbw[2]=0xFFFF*b*b;
//  rgbw[3]=0xFFFF*w*w;
}

// source: http://museinfo.sapp.org/examples/humdrum/keyscape2.cpp
void hsi2rgb(float H, float S, float I, int* rgb) {
   float r;
   float g;
   float b;

   if (H < 1.0/3.0) {
      b = (1-S)/3;
      r = (1+S*cos(2*PI*H)/cos(PI/3-2*PI*H))/3.0;
      g = 1 - (b + r);
   } else if (H < 2.0/3.0) {
      H = H - 1.0/3.0;
      r = (1-S)/3;
      g = (1+S*cos(2*PI*H)/cos(PI/3-2*PI*H))/3.0;
      b = 1 - (r+g);
   } else {
      H = H - 2.0/3.0;
      g = (1-S)/3;
      b = (1+S*cos(2*PI*H)/cos(PI/3-2*PI*H))/3.0;
      r = 1 - (g+b);
   }

   rgb[0] = (int)((I * r * 3)+.5);
   rgb[1] = (int)((I * g * 3)+.5);
   rgb[2] = (int)((I * b * 3)+.5);
}

