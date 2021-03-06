/*
    Helloworld.ino - Example for KNX module (KIM) library

    Send to KNX BUS the follow date:
        - button status (1.001);
        - led status (1.001).

    Recive to KNX BUS the follow date:
        - button toggle status (1.001);
        - led command (1.001).

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

#define OBJ_CMD_LED       0
#define OBJ_ST_LED        1
#define OBJ_CMD_BUTTON    2
#define OBJ_ST_BUTTON     3

KIMaip knxIno(KNX_DATAREADY, KNX_BUS);
DPT cmdLed(OBJ_CMD_LED, &knxIno);
DPT statLed(OBJ_ST_LED, &knxIno);
DPT cmdButton(OBJ_CMD_BUTTON, &knxIno);
DPT statButton(OBJ_ST_BUTTON, &knxIno);

// variable for reading the pushbutton status
bool oldButtonState = false;
bool buttonPressed = true;
bool oldLed = false;
bool oldStatButtonKNX = false;

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  pinMode(BUTTON, INPUT_PULLUP);
}

void loop() {

  bool newStatButtonKNX;
  bool ledStatus;

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if ((digitalRead(BUTTON) == LOW) && (buttonPressed == false)) {
    buttonPressed = true;
    oldButtonState = !oldButtonState;
    cmdButton.setValue(oldButtonState);
  }

  if (digitalRead(BUTTON) == HIGH) {
    buttonPressed = false;
  }

  if (oldLed != digitalRead(LED)){
    oldLed = !oldLed;
    statLed.setValue(oldLed);
  }

  if (knxIno.recive()) {
    cmdLed.getValue(ledStatus);
    digitalWrite(LED, ledStatus);
    statButton.getValue(newStatButtonKNX);
    if (oldStatButtonKNX != newStatButtonKNX) {
      statButton.getValue(oldButtonState);
      statButton.getValue(oldStatButtonKNX);
    }
  }

  statLed.responseValue(ledStatus);
  cmdButton.responseValue(oldButtonState);
}
