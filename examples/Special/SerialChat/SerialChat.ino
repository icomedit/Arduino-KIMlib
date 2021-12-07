/*
  SerialChat.ino - Serial Chat example

  When new serial data arrives, this sketchadds it send data to KNX bus.
  When a newline is received, the loop prints the string and clears it.

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
#define TEXT_LEN          14
#define OBJ_TEXT          29

KIMaip knxIno(KNX_DATAREADY, KNX_BUS);
DPT dpt_text(OBJ_TEXT, &knxIno);

String inputString = "";         // a String to hold incoming data
char bufTX[TEXT_LEN];
char bufRX[TEXT_LEN];
bool stringComplete = false;  // whether the string is complete

void setup() {
  // initialize serial:
  Serial.begin(SERIAL_BIT_RATE);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  cleanArray(bufTX, TEXT_LEN);
  cleanArray(bufRX, TEXT_LEN);
  Serial.println(F("\nSetting \"CR\" to serial monitor."));
  Serial.println(F("Text to send:"));
}

void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {
    inputString.getBytes(bufTX, TEXT_LEN + 1);
    Serial.print(F("<TX:"));

    for (int i = inputString.length() - 1; i < TEXT_LEN; i++) {
      bufTX[i] = ' ';
    }

    memcpy (knxIno.buf, bufTX, TEXT_LEN);

    printArray(knxIno.buf, TEXT_LEN);
    knxIno.sendValue(OBJ_TEXT, TEXT_LEN);
    Serial.print(F("| "));
    cleanArray(bufTX, TEXT_LEN);
    Serial.println(inputString);

    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  if (knxIno.recive()) {
    dpt_text.getValue(bufRX);
    if (bufRX[TEXT_LEN - 1] != '\0') {
      Serial.print(F("RX>:"));
      printArray(bufRX, TEXT_LEN);
      Serial.print(F("|"));
      for (int i = TEXT_LEN; i >= 0; i--) {
        Serial.print(bufRX[i]);
      }
      Serial.println();
      cleanArray(bufRX, TEXT_LEN);
    }
  }
  dpt_text.responseValue(bufRX);
}

void systemEvent(unsigned long sysEvent)
{
  union l_tag {
    unsigned long temp_long ;
    unsigned int temp_int[2] ;
	} l;

  l.temp_long = sysEvent;
  unsigned int idEvent = l.temp_int[1];
  unsigned int objN = l.temp_int[0];
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();

    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if ((inChar == '\n') || (inChar == '\r')) {
      stringComplete = true;
    }
  }
}


//print the array
void printArray(char arr[], size_t size)
{
    for (size_t i=0; i < size; i++)
    {
      if (i > 0) Serial.print(F(" "));
      Serial.print(arr[i], HEX);
    }
}

void cleanArray(char arr[], size_t size)
{
    for (size_t i=0; i < size; i++)
    {
        arr[i] = '\0';
    }
}
