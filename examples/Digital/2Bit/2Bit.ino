/*
    2Bit.ino - Example for KNX module (KIM) library
    Copyright (C) 2021  Fabio Di MIchele

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
#define LED_GREEN        10
#define BUTTON            8     // Pin pulsante S3

// Object definition scope in ETS exacly sequnce respect
#define OBJ_2            14     // 2Bit Write da BUS
#define OBJ_3            15     // 2Bit Read da BUS

#define VALUE             0	    //LSB
#define BLOCK             1

struct Switch_Control {
  uint8_t buffer:2;
} switchControl;

KIMaip knxIno(KNX_DATAREADY, KNX_BUS);
DPT oby_2(OBJ_2, &knxIno);
DPT oby_3(OBJ_3, &knxIno);

// variables will change:
bool buttonPressed = true;
byte index = 0;

void setup() {
  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_RED, LOW);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, LOW);
  pinMode(BUTTON, INPUT_PULLUP);
}

void loop() {
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if ((digitalRead(BUTTON) == LOW) && (buttonPressed == false)) {
    buttonPressed = true;
    if (index > 3) index=0;
    bitWrite(switchControl.buffer, VALUE, bitRead(index, VALUE));
    bitWrite(switchControl.buffer, BLOCK, bitRead(index, BLOCK));
    oby_3.setValue(switchControl);
    index++;
  }

  if (digitalRead(BUTTON) == HIGH) {
    buttonPressed = false;
  }

  if (knxIno.recive()) {
    oby_2.getValue(switchControl);
    digitalWrite(LED_RED, bitRead(switchControl.buffer, VALUE));
    digitalWrite(LED_GREEN, bitRead(switchControl.buffer, BLOCK));
  }

  oby_3.responseValue(switchControl);
}
