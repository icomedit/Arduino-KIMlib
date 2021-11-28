/*
    8Bit.ino - Example for KNX module (KIM) library
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

#define BUTTON            8     // Pin pulsante S3

// Object definition scope in ETS exacly sequnce respect
#define OBJ_6             6  // 8Bit Write da BUS
#define OBJ_7             7  // 8Bit Read da BUS

#define SERIAL_BIT_RATE   115200 // Velocità della seriale

struct scene_control {
  uint8_t buffer;
} sceneControl;

KIMaip knxIno(KNX_DATAREADY, KNX_BUS);
DPT oby_6(OBJ_6, &knxIno);
DPT oby_7(OBJ_7, &knxIno);

// variables will change:
bool buttonPressed = true;

void setup() {
  Serial.begin(SERIAL_BIT_RATE);
  pinMode(BUTTON, INPUT_PULLUP);
}

void loop() {

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if ((digitalRead(BUTTON) == LOW) && (buttonPressed == false)) {
    buttonPressed = true;
    if (sceneControl.buffer > 255) sceneControl.buffer = 0;
    oby_6.setValue(sceneControl);
    sceneControl.buffer++;
  }

  if (digitalRead(BUTTON) == HIGH) {
    buttonPressed = false;
  }

  if (knxIno.recive()) {
    oby_7.getValue(sceneControl);
    Serial.println(sceneControl.buffer, HEX);
  }

  oby_6.responseValue(sceneControl);
  delay(100);
}
