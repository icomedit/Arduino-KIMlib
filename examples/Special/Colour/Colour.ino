/*
    Colour.ino - Example for KNX module (KIM) library

    Recive 6 bytes to KNX BUS for konnex RGBW value (251.600).
    Also it is implement responce to konnex BUS request.

    Circuit:
    You can buy KIMaip KNX / EIB shield for your experiment whit Arduino.
    See the link: https://www.ebay.it/itm/324815210159

    Copyright (C) 2021  Fabio Di Michele
    Copyright (C) 2021  Giulio Paggi

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <KIMlib.h>

#define KNX_DATAREADY     2      // Pin data ready KNX
#define KNX_BUS           12     // Status BUS KNX

#define LED_RED           10     // Pin Red   on RGB LED
#define LED_GREEN         6      // Pin Green on RGB LED
#define LED_BLUE          5      // Pin Blue  on RGB LED

#define OBJ_RGBW_LED      26

#define SERIAL_BIT_RATE   115200 // Serial monitor speed
#define WAIT              1000

KIMaip knxIno(KNX_DATAREADY, KNX_BUS);
DPT dpt_rgbwLed(OBJ_RGBW_LED, &knxIno);

// variables will change:
struct Colour_RGBW {
  byte lsb = B00001111;
  byte reserved = 0;
  byte white = 0;
  byte blue = 0;
  byte green = 80;
  byte red = 125;
} colourRGBW;

unsigned long old_millis = 0;

void setup() {
  Serial.begin(SERIAL_BIT_RATE);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  Serial.println(F("\r"));
}

void loop() {
  if ((millis() - old_millis) > WAIT) {
    dpt_rgbwLed.setValue(colourRGBW);
    Serial.print(F("PWM LED: "));
    Serial.print(colourRGBW.red);
    Serial.print(F(" "));
    Serial.print(colourRGBW.green);
    Serial.print(F(" "));
    Serial.print(colourRGBW.blue);
    Serial.print(F(" "));
    Serial.print(colourRGBW.white);
    Serial.print(F(" "));
    Serial.print(colourRGBW.lsb, HEX);
    colourRGBW.red++;
    colourRGBW.green++;
    colourRGBW.blue++;
    Serial.println();
    old_millis = millis();
  }

  if (knxIno.recive()) {
    dpt_rgbwLed.getValue(colourRGBW);
    analogWrite(LED_RED, colourRGBW.red);
    analogWrite(LED_GREEN, colourRGBW.green);
    analogWrite(LED_BLUE, colourRGBW.blue);
  }

  dpt_rgbwLed.responseValue(colourRGBW);
}
