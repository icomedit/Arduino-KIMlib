/*
    Rele.ino - Example for KNX module (KIM) library
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

#define RELAY_1           7     // Pin relay 1
#define RELAY_2           6     // Pin relay 2
#define RELAY_3           5     // Pin relay 3
#define RELAY_4           4     // Pin relay 4

#define KNX_DATAREADY     2     // Pin 1 Gateway KNX
#define KNX_BUS           12    // Stato BUS KNX

// Object definition scope in ETS exacly sequnce respect
#define OBJ_RELE1     4
#define STAT_RELE1    5
#define OBJ_RELE2     6
#define STAT_RELE2    7
#define OBJ_RELE3     8
#define STAT_RELE3    9
#define OBJ_RELE4     10
#define STAT_RELE4    11

KIMaip knxIno(KNX_DATAREADY, KNX_BUS);
DPT rele1(OBJ_RELE1, &knxIno);
DPT statRele1(STAT_RELE1, &knxIno);
DPT rele2(OBJ_RELE2, &knxIno);
DPT statRele2(STAT_RELE2, &knxIno);
DPT rele3(OBJ_RELE3, &knxIno);
DPT statRele3(STAT_RELE3, &knxIno);
DPT rele4(OBJ_RELE4, &knxIno);
DPT statRele4(STAT_RELE4, &knxIno);

bool newRele1 = false;
bool newRele2 = false;
bool newRele3 = false;
bool newRele4 = false;
bool oldRele1 = false;
bool oldRele2 = false;
bool oldRele3 = false;
bool oldRele4 = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(RELAY_1, OUTPUT);
  digitalWrite(RELAY_1, LOW);
  pinMode(RELAY_2, OUTPUT);
  digitalWrite(RELAY_2, LOW);
  pinMode(RELAY_3, OUTPUT);
  digitalWrite(RELAY_3, LOW);
  pinMode(RELAY_4, OUTPUT);
  digitalWrite(RELAY_4, LOW);

   rele1.getStatusValue();
   while (knxIno.isReadBusy()) {
    if (knxIno.recive()) {
      rele1.getValue(newRele1);
      digitalWrite(RELAY_1, newRele1);
    }
   }

   rele2.getStatusValue();
   while (knxIno.isReadBusy()) {
    if (knxIno.recive()) {
      rele2.getValue(newRele2);
      digitalWrite(RELAY_2, newRele2);
    }
   }

   rele3.getStatusValue();
   while (knxIno.isReadBusy()) {
    if (knxIno.recive()) {
      rele3.getValue(newRele3);
      digitalWrite(RELAY_3, newRele3);
    }
   }

   rele4.getStatusValue();
   while (knxIno.isReadBusy()) {
    if (knxIno.recive()) {
      rele4.getValue(newRele4);
      digitalWrite(RELAY_4, newRele4);
    }
   }
}

void loop() {
  // put your main code here, to run repeatedly:

  if (knxIno.recive()) {
    rele1.getValue(newRele1);
    digitalWrite(RELAY_1, newRele1);
    if (oldRele1 != digitalRead(RELAY_1)){
      oldRele1 = !oldRele1;
      statRele1.setValue(oldRele1);
    }

	  rele2.getValue(newRele2);
    digitalWrite(RELAY_2, newRele2);
    if (oldRele2 != digitalRead(RELAY_2)){
      oldRele2 = !oldRele2;
      statRele2.setValue(oldRele2);
    }
    rele3.getValue(newRele3);
    digitalWrite(RELAY_3, newRele3);
    if (oldRele3 != digitalRead(RELAY_3)){
      oldRele3 = !oldRele3;
      statRele3.setValue(oldRele3);
    }
    rele4.getValue(newRele4);
    digitalWrite(RELAY_4, newRele4);
    if (oldRele4 != digitalRead(RELAY_4)){
      oldRele4 = !oldRele4;
      statRele4.setValue(oldRele4);
    }

    statRele1.responseValue(oldRele1);
    statRele2.responseValue(oldRele2);
	  statRele3.responseValue(oldRele3);
    statRele4.responseValue(oldRele4);
  }
}
