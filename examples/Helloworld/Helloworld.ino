/*
    Helloworld.ino - Example for KNX module (KIM) library
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

#include "kimlib.h"

#define KNX_DATAREADY     2     // Pin data ready KNX
#define KNX_BUS          12     // Pin BUS KNX OK

#define LED              13     // Pin LED_BUILTIN
#define PULSANTE          8     // Pin pulsante S3

// Object definition scope in ETS exacly sequnce respect
#define OBJ_CMD_LED       0
#define OBJ_ST_LED        1
#define OBJ_CMD_BUTTON    2
#define OBJ_ST_BUTTON     3

KIMaip knxIno(KNX_DATAREADY, KNX_BUS);
DPT<boolean> cmdLed(OBJ_CMD_LED, &knxIno);
DPT<boolean> statLed(OBJ_ST_LED, &knxIno);
DPT<boolean> cmdButton(OBJ_CMD_BUTTON, &knxIno);
DPT<boolean> statButton(OBJ_ST_BUTTON, &knxIno);

// variables will change:
bool oldButtonState = false;         // variable for reading the pushbutton status
bool buttonPressed = true;
bool oldLed = false;
bool oldStatButtonKNX = false;

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  pinMode(PULSANTE, INPUT_PULLUP); 
}

void loop() {

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if ((digitalRead(PULSANTE) == LOW) && (buttonPressed == false)) {    
    buttonPressed = true;
    oldButtonState = !oldButtonState;
    cmdButton.setValue(oldButtonState);    
  } 
  
  if (digitalRead(PULSANTE) == HIGH) {
    buttonPressed = false;
  }

  if (oldLed != digitalRead(LED)){
    oldLed = !oldLed;
    statLed.setValue(oldLed);    
  }
  
  if (knxIno.recive()) {
    digitalWrite(LED, cmdLed.getValue());
    if (oldStatButtonKNX != statButton.getValue()) {
      oldButtonState = statButton.getValue();
      oldStatButtonKNX = statButton.getValue();
    }
  }

  statLed.responseValue(digitalRead(LED));
  cmdButton.responseValue(oldButtonState);
}
