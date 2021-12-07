/*
    1Bit.ino - Example for KNX module (KIM) library

    Send 1 bit to KNX BUS status of the internal button (1.001).
    Recive 1 bit to KNX BUS command to builtin led (1.001).
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

#define KNX_DATAREADY     2     // Pin data ready KNX
#define KNX_BUS          12     // Status BUS KNX

#define LED              13     // Pin LED_BUILTIN
#define BUTTON            8     // Pin button S3

// Object definition scope in ETS exacly sequnce respect
#define OBJ_0            12     // 1Bit Write da BUS
#define OBJ_1            13     // 1Bit Read da BUS

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
