/*
    1Bit.ino - Example for KNX module (KIM) library
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

#include <kimlib.h>

#define KNX_DATAREADY     2     // Pin data ready KNX
#define KNX_BUS          12     // Pin BUS KNX OK

#define LED              13     // Pin LED_BUILTIN
#define BUTTON            8     // Pin pulsante S3

// Object definition scope in ETS exacly sequnce respect
#define OBJ_0             0  // 1Bit Write da BUS
#define OBJ_1             1  // 1Bit Read da BUS

struct Switch_Control {
  bool status = false;
} switchControl;

KIMaip knxIno(KNX_DATAREADY, KNX_BUS);
DPT oby_0(OBJ_0, &knxIno);
DPT oby_1(OBJ_1, &knxIno);

// variables will change:
bool buttonPressed = true;

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  pinMode(BUTTON, INPUT_PULLUP); 
}

void loop() {
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if ((digitalRead(BUTTON) == LOW) && (buttonPressed == false)) {    
    buttonPressed = true;
    switchControl.status = !switchControl.status;
    oby_1.setValue(switchControl);
  } 
  
  if (digitalRead(BUTTON) == HIGH) {
    buttonPressed = false;
  }

  if (knxIno.recive()) {
    oby_0.getValue(switchControl);
    digitalWrite(LED, switchControl.status);
  }

  oby_1.responseValue(switchControl);
  delay(100);
}
