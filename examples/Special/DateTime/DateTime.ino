/*
    DateTime.ino - Example for KNX module (KIM) library
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
#include "calendarClock.h"

#define KNX_DATAREADY     2      // Pin data ready KNX
#define KNX_BUS           12     // Pin BUS KNX OK

#define LED_GREEN         10     // Pin LED_BUILTIN

// Object definition scope in ETS exacly sequnce respect
#define OBJ_DATETIME      27

#define SERIAL_BIT_RATE   115200 // Velocità della seriale
#define WAIT              1000

KIMaip knxIno(KNX_DATAREADY, KNX_BUS);
DPT dpt_dateTime(OBJ_DATETIME, &knxIno);

// variables will change:
unsigned long old_millis = 0;

void setup() {
  Serial.begin(SERIAL_BIT_RATE);  // Inizializza Seriale
  pinMode(LED_GREEN, OUTPUT);
  Serial.println(F("\r"));
  compileTime();
}

void loop() {
  if ((millis() - old_millis) > WAIT) {
    tickClock();
    digitalWrite(LED_GREEN, ((datetime.seconds & 1) == 0));
    dpt_dateTime.setValue(datetime);
    digitalClockDisplay();
    old_millis = millis();    
  }
 
  if (knxIno.recive()) {    
    dpt_dateTime.getValue(datetime);    
  }

  dpt_dateTime.responseValue(datetime);  
}
