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
#include "conversion.h"

// Sonda di temperatura e umidità
#include <AM232X.h>

#define KNX_DATAREADY     2      // Pin data ready KNX
#define KNX_BUS           12     // Pin BUS KNX OK

#define LED               10     // Pin LED_BUILTIN
#define AIN               A2      // Pin analog input

// Object definition scope in ETS exacly sequnce respect
#define OBJ_PWD_LED       0
#define OBJ_T             1
#define OBJ_H             2
#define OBJ_AIN_BYTE      3
#define OBJ_AIN_WORD      4
#define OBJ_UPTIME        5

#define SERIAL_BIT_RATE   115200 // Velocità della seriale
#define WAIT              10 

KIMaip knxIno(KNX_DATAREADY, KNX_BUS);
DPT dpt_pwmLed(OBJ_PWD_LED, &knxIno);
DPT dpt_temperature(OBJ_T, &knxIno);
DPT dpt_humidity(OBJ_H, &knxIno);
DPT dpt_a2byte(OBJ_AIN_BYTE, &knxIno);
DPT dpt_a2word(OBJ_AIN_WORD, &knxIno);
DPT dpt_uptime(OBJ_UPTIME, &knxIno);

UserParameter up_txTime(&knxIno);  // secondi da 1 a 255 , 0 = OFF 
UserParameter up_delta(&knxIno);   // 0.1 da 0.1 a 25.5 , 0 = OFF

AM232X AM2322;

// variables will change:
byte  pwmLed          = 0;
float delta           = 0.0;
unsigned long time_ms = 0;
unsigned long old_millis = 0;
word i = 0;

void setup() {
  Serial.begin(SERIAL_BIT_RATE);  // Inizializza Seriale
  pinMode(LED, OUTPUT);
  Serial.println(F("\r"));
  
  if (! AM2322.begin() ) {
    Serial.println(F("Sensor HT not found"));
  }
  delay(WAIT * 5);
  time_ms = (unsigned long)up_txTime.getValue() * 1000.0;
  Serial.print(F("elapse ms:\t"));
  Serial.println(time_ms);
  delay(WAIT * 5);
  delta = (float)up_delta.getValue() / 10.0;
  Serial.print(F("delta:\t\t"));
  Serial.println(delta);
  delay(WAIT * 50);
  old_millis = millis();
  Serial.println();
}

void loop() {
  
 if (time_ms != 0) {
    if ((millis() - old_millis) > time_ms) {
      // Read temperature as Celsius
      AM2322.read();
      float t = AM2322.getTemperature();
      Serial.print(F("temperature:\t"));
      Serial.println(t);
      uint16_t th = float2half(t);
      dpt_temperature.setValue(th);
      float h = AM2322.getHumidity();
      Serial.print(F("humidity:\t"));
      Serial.println(h);
      uint16_t hh = float2half(h);
      dpt_humidity.setValue(hh);
      unsigned int sensVal = analogRead(AIN);
      byte value_byte = map(sensVal, 0, 1024, 0, 255);
      dpt_a2byte.setValue(value_byte);
      if (i > 65535) i = 0;
      word value_word = i++;
      dpt_a2word.setValue(value_word);           
      old_millis = millis();
      Serial.print(F("millis:\t\t0x"));
      Serial.println(old_millis , HEX);
      dpt_uptime.setValue(old_millis); 
      Serial.print(F("pulse:\t\t"));
      Serial.println(i);
      Serial.println();
    }
  }
 
  if (knxIno.recive()) {
#ifdef _DEBUG
    systemEvent(knxIno.getSystemEvent());
    delay(WAIT);
#endif    
    dpt_pwmLed.getValue(pwmLed);
    analogWrite(LED, pwmLed);
  }

  dpt_pwmLed.responseValue(pwmLed);  
}

static String printObj(unsigned int id)
{
  switch (id) {
    case OBJ_PWD_LED:           
        return "Button";
        break;
    case OBJ_T:           
        return "Tempereature";
        break;
    case OBJ_H:           
        return "Umidity";
        break;
    case OBJ_AIN_BYTE:           
        return "Analog IN Byte";
        break;
    case OBJ_AIN_WORD:           
        return "Analog IN Word";
        break;
    case OBJ_UPTIME:           
        return "uptime";
        break;
  } 
}

void systemEvent(unsigned long sysEvent)
{
  union l_tag {                             // Siccome la temperatura è un dato FLOAT, si usa la funzione Union per "spacchettare" i 4 BYTE che la compongono.
    unsigned long temp_long ;               // Se copi dentro "u.temp_float" un valore float automaticamente ottieni i relativi quattro byte nel array "u.temp_byte[n]", con n compreso tra 0 e 3,
    unsigned int temp_int[2] ;              // Viceversa se copy nel array i quattro byte, ottieni il tuo valore float dentro u.temp_float.
  } l;
  l.temp_long = sysEvent;
  unsigned int idEvent = l.temp_int[1];
  unsigned int objN = l.temp_int[0];

  Serial.print(F("Sytem Event: 0x"));
  Serial.println(l.temp_long, HEX);
  Serial.print(printObj(objN));
  Serial.println(printIdEvent(idEvent));
}
