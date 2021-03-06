/*
 KIMaipInfo.ino - Example for KNX module (KIM) library

 This example check if the firmware loaded on the TAPKO KIMaip
 module is updated.

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

#define KNX_DATAREADY     2      // Pin data ready KNX
#define KNX_BUS           12     // Status BUS KNX

#define SERIAL_BIT_RATE   115200 // Serial monitor speed
#define WAIT              10

KIMaip knxIno(KNX_DATAREADY, KNX_BUS);

char* sPriorityMode[3] {"ETS", "I2C", "Default"};
char* sBool[2] {"NO ", "YES"};

void setup() {
  // Initialize serial
  Serial.begin(SERIAL_BIT_RATE);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Print a welcome message
  Serial.println(F("KIMaip firmware check."));
  Serial.println();

  // Check for the presence of the shield
  Serial.print(F("KIMaip Module: "));
  if (!knxIno.isBusKNXready()) {
    Serial.println(F("NOT PRESENT"));
    return; // don't continue
  }
  Serial.println(F("DETECTED"));

  //do something when var equals 0001h
  Serial.print(F("Version: "));
  Serial.print(knxIno.getVersion());
  Serial.println();
  delay(WAIT);
  //do something when var equals 0002h
  Serial.print(F("KNX Address: "));
  word address = knxIno.getKNXaddress();
  byte digit[3];

  digit[2] = (byte) ((address & 0xFF00) >> 8);
  digit[1] = (byte) ((address & 0x00F0) >> 4);
  digit[0] = (byte) ((address & 0x000F));
  for (int i = 0; i < 3; i++) {
    if (i > 0) Serial.print(F("."));
    Serial.print(digit[i], DEC);
  }
  Serial.println();
  delay(WAIT);
  //do something when var equals 0003h
  Serial.print(F("Programming Mode: "));
  Serial.print(sPriorityMode[knxIno.getProgMode()]);
  Serial.println();
  delay(WAIT);
  //do something when var equals 0005h
  Serial.print(F("I2C Address to: "));
  Serial.print(sPriorityMode[knxIno.getI2Caddress()]);
  Serial.println();
  delay(WAIT);
  //do something when var equals 0006h
  Serial.print(F("Actual I2C Address: 0x"));
  Serial.print(knxIno.getI2CacutalAddress(), HEX);
  Serial.println();
  delay(WAIT);
  //do something when var equals 0011h
  Serial.print(F("Bit to Enable System-Event: "));
  Serial.print(knxIno.getSYSevent(), BIN);
  Serial.println();
  delay(WAIT);
  //do something when var equals 0012h
  Serial.print(F("Enable Error-Message: "));
  Serial.print(sBool[knxIno.getEnableError()]);
  Serial.println();
  delay(WAIT);
  //do something when var equals 0020h
  Serial.print(F("TimeOut I2C message: "));
  Serial.print(knxIno.getI2Ctimeout());
  Serial.print(F(" ms"));
  Serial.println();
  delay(WAIT);
  //do something when var equals 0021h
  Serial.print(F("TimeOut indication: "));
  Serial.print(knxIno.getIndTimeout());
  Serial.print(F(" ms"));
  Serial.println();
  delay(WAIT);
  //do something when var equals 0022h
  Serial.print(F("TimeOut indication no Master: "));
  Serial.print(knxIno.getMasterTimeout());
  Serial.print(F(" ms"));
  Serial.println();
  delay(WAIT);
  //do something when var equals 0023h
  Serial.print(F("Retry sending: "));
  Serial.print(knxIno.getRtySend());
  Serial.println();
  delay(WAIT);
  //do something when var equals 0100h
  Serial.print(F("Setting for TrasparentMode: "));
  Serial.print(knxIno.getTransMode(), BIN);
  Serial.println();
  delay(WAIT);
  //do something when var equals 0110h
  Serial.print(F("GroupLink Disabled: "));
  Serial.print(sBool[knxIno.getGroupLink()]);
  Serial.println();
  delay(WAIT);
  //do something when var equals 0200h
  Serial.print(F("State Device: "));
  if (knxIno.getDevStatus() == KIM_READY) {
    Serial.print(F("Normal mode"));
  } else {
    Serial.print(knxIno.getDevStatus(), BIN);
  }
  Serial.println();
  delay(WAIT);
  Serial.flush();
}

void loop() {
  // do nothing
}
