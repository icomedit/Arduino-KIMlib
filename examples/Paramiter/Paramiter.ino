/*
    Helloworld.ino - Example for KNX module (KIM) library
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

#define KNX_DATAREADY     2      // Pin data ready KNX
#define KNX_BUS           12     // Pin BUS KNX OK

#define LED               10     // Pin LED_BUILTIN
#define AIN               A2      // Pin analog input

// Object definition scope in ETS exacly sequnce respect
#define OBJ_PWD_LED       20
#define OBJ_AIN_BYTE      23

#define SERIAL_BIT_RATE   115200 // Velocità della seriale

KIMaip knxIno(KNX_DATAREADY, KNX_BUS);
DPT dpt_a2byte(OBJ_AIN_BYTE, &knxIno);

// Paramatri utilizzati per la gestione dell'invio ciclico o su variazione del valore di A2
UserParameter up_txTime(&knxIno);  // secondi di ritrasmissione da 1 a 255 , 0 = OFF, non ritrasmette 
UserParameter up_delta(&knxIno);   // da 1 a 255 , 0 = OFF

// variables will change:
byte delta = 0;
unsigned long time_ms = 0;
unsigned long old_millis = 0;
byte value = 0;
byte oldValue = 0;

void setup() {
  Serial.begin(SERIAL_BIT_RATE);  // Inizializza Seriale
  pinMode(LED, OUTPUT);
  Serial.println(F("\r"));
  
  time_ms = (unsigned long)up_txTime.getValue() * 1000.0;
  Serial.print(F("elapse:\t\t"));
  Serial.print(time_ms);
  Serial.println(F(" msec"));

  delta = up_delta.getValue();
  Serial.print(F("delta:\t\t"));
  Serial.print(delta);
  Serial.println(F(" +/-"));
  delay(100);

  old_millis = millis();
  Serial.println();
}

void loop() {

  unsigned int sensVal = analogRead(AIN);
  value = map(sensVal, 0, 1024, 0, 255);
  analogWrite(LED, value);
  
  if (time_ms != 0) {
    if ((millis() - old_millis) > time_ms) {

      dpt_a2byte.setValue(value);
      Serial.print(F("Send Cyclic A2:\t"));
      Serial.println(value);
      oldValue = value; // Aggiorna valore per invio
      
      Serial.println();
      old_millis = millis();
    }
  }
    
   if (delta != 0) {
    int constValue = constrain(value, oldValue - delta, oldValue + delta);
   
      if ( constValue != value ) {      
        dpt_a2byte.setValue(value);
        Serial.print(F("Send Changend A2:\t"));
        Serial.println(value);
        oldValue = value;
        old_millis = millis(); // Aggiorna timeout per invio
      }
   }
   
  dpt_a2byte.responseValue(value);
}
