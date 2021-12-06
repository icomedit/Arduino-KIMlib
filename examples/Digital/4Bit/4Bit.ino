/*
    4Bit.ino - Example for KNX module (KIM) library
    Copyright (C) 2021  Fabio Di MIchele
    Copyright (C) 2021  Giulio Paggi

    You can buy KIMaip KNX / EIB shield for your experiment whit Arduino.
    See the link: https://www.ebay.it/itm/324815210159
    
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

#define KNX_DATAREADY     2     // Pin data ready KNX
#define KNX_BUS          12     // Pin BUS KNX OK

#define LED_RED          13     // Pin LED_BUILTIN
#define LED_PWM          10
#define BUTTON            8     // Pin pulsante S3

// Object definition scope in ETS exacly sequnce respect
#define OBJ_4            16     // 4Bit Write da BUS
#define OBJ_5            17     // 4Bit Read da BUS

#define INTERVAL_0        0
#define INTERVAL_1        1
#define INTERVAL_2        2
#define DIRECTION         3

struct Control_Dimming {
  uint8_t buffer:4;
} controlDimming;

KIMaip knxIno(KNX_DATAREADY, KNX_BUS);
DPT oby_4(OBJ_4, &knxIno);
DPT oby_5(OBJ_5, &knxIno);

// variables will change:
bool buttonPressed = true;
byte index = 0;
byte pwm = 0;

void setup() {
  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_RED, LOW);
  pinMode(LED_PWM, OUTPUT);
  analogWrite(LED_PWM, pwm);
  pinMode(BUTTON, INPUT_PULLUP);
}

void loop() {
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if ((digitalRead(BUTTON) == LOW) && (buttonPressed == false)) {
    buttonPressed = true;
    if (index > 15) index=0;
    bitWrite(controlDimming.buffer, INTERVAL_0, bitRead(index, INTERVAL_0));
    bitWrite(controlDimming.buffer, INTERVAL_1, bitRead(index, INTERVAL_1));
    bitWrite(controlDimming.buffer, INTERVAL_2, bitRead(index, INTERVAL_2));
    bitWrite(controlDimming.buffer, DIRECTION, bitRead(index, DIRECTION));
    oby_5.setValue(controlDimming);
    index++;
  }

  if (digitalRead(BUTTON) == HIGH) {
    buttonPressed = false;
  }

  if (knxIno.recive()) {
    oby_4.getValue(controlDimming);
    digitalWrite(LED_RED, bitRead(controlDimming.buffer, DIRECTION));
    pwm = controlDimming.buffer & 0x7;
    pwm = map(pwm, 0, 15, 0, 255);
    analogWrite(LED_PWM, pwm);
  }

  oby_5.responseValue(controlDimming);
}
