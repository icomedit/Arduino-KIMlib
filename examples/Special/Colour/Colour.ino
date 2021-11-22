/*
    Colour.ino - Example for KNX module (KIM) library
    Copyright (C) 2021  Fabio Di MIchele
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

#include <kimlib.h>

#define KNX_DATAREADY     2      // Pin data ready KNX
#define KNX_BUS           12     // Pin BUS KNX OK

#define LED_GREEN         10     // Pin LED_BUILTIN

// Object definition scope in ETS exacly sequnce respect
#define OBJ_RGBW_LED      26

#define SERIAL_BIT_RATE   115200 // Velocità della seriale
#define WAIT              5000

KIMaip knxIno(KNX_DATAREADY, KNX_BUS);
DPT dpt_rgbwLed(OBJ_RGBW_LED, &knxIno);

// variables will change:
struct Colour_RGBW {
  byte lsb = B00001111;
  byte reserved = 0;
  byte white = 0;
  byte blue = 0;
  byte green = 0;
  byte red = 0;
} colourRGBW;

unsigned long old_millis = 0;

void setup() {
  Serial.begin(SERIAL_BIT_RATE);  // Inizializza Seriale
  pinMode(LED_GREEN, OUTPUT);
  Serial.println(F("\r"));
}

void loop() {
  if ((millis() - old_millis) > WAIT) {
    dpt_rgbwLed.setValue(colourRGBW);
    Serial.print(F("PWM LED:\t"));
    Serial.println(colourRGBW.green);      
    colourRGBW.green++;
    Serial.println();
    old_millis = millis();      
  }
 
  if (knxIno.recive()) {
    dpt_rgbwLed.getValue(colourRGBW);
    analogWrite(LED_GREEN, colourRGBW.green);
  }

  dpt_rgbwLed.responseValue(colourRGBW);  
}
