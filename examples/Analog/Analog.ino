/*
    Analog.ino - Example for KNX module (KIM) library

    Send to KNX BUS the follow date:
        - temperature (9.001) random values;
        - humidity (9.007) random values;
        - analog input in a byte (5.005);
        - counter in 2 bytes (7.001);
        - uptime in 4 bytes (14.074).

    RRecive to KNX BUS the follow values
        - temperature (9.001);
        - humidity (9.007);
        - drive led in PWM (5.001).

    Also it read the follow KNX user parameters form KIMaip device:
        - P0 = timeout
        - P1 = delta

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

#define KNX_DATAREADY     2      // Pin data ready KNX
#define KNX_BUS           12     // Status BUS KNX

#define LED               10     // Pin LED_BUILTIN
#define AIN               A2     // Pin analog input

// Object definition scope in ETS comunication object
#define OBJ_PWD_LED       20
#define OBJ_T             21
#define OBJ_H             22
#define OBJ_AIN_BYTE      23
#define OBJ_COUNTER       24
#define OBJ_UPTIME        25

#define SERIAL_BIT_RATE   115200 // Serial monitor speed
#define WAIT              10

KIMaip knxIno(KNX_DATAREADY, KNX_BUS);
DPT dpt_pwmLed(OBJ_PWD_LED, &knxIno);
DPT dpt_temperature(OBJ_T, &knxIno);
DPT dpt_humidity(OBJ_H, &knxIno);
DPT dpt_a2byte(OBJ_AIN_BYTE, &knxIno);
DPT dpt_counter(OBJ_COUNTER, &knxIno);
DPT dpt_uptime(OBJ_UPTIME, &knxIno);

// KNX parameter in sequence start on user parameter number 0
// Object definition scope in ETS exacly sequnce respect
UserParameter up_txTime(&knxIno);  // P0 - time out for re-trasmittion from 1 to 255 seconds; 0 = OFF, not re-trasmittion
UserParameter up_delta(&knxIno);   // P1 - delta for re-trasmittion from 1 to 255 , 0 = OFF, not re-trasmittion

// variables will change:
byte pwmLed = 0;
unsigned long time_ms = 0;
unsigned long oldMillis = 0;
float sec = 0;
word i = 0;
uint16_t th = 0;
uint16_t hh = 0;
byte value = 0;
float oldValue = 0;
float delta = 0.0;
float t = 10.0;

void setup() {
  Serial.begin(SERIAL_BIT_RATE);
  pinMode(LED, OUTPUT);
  randomSeed(analogRead(AIN));
  delay(WAIT * 5);
  time_ms = (unsigned long)up_txTime.getValue() * 1000.0;
  Serial.print(F("elapse ms:\t"));
  Serial.println(time_ms);
  delay(WAIT * 5);
  delta = (float)up_delta.getValue() / 10.0;
  Serial.print(F("delta:\t\t"));
  Serial.println(delta);
  delay(WAIT * 50);
  oldMillis = millis();
  Serial.println();
}

void loop() {
  // simulation temperature values
  if (random(0,2) == 1) {
    t = t + 0.05;
  } else {
    t = t - 0.05;
  }
  // disabled if paramiterss are zero 
  if ((time_ms != 0) && (delta != 0)) {
    // compare the time or delta
    if (((millis() - oldMillis) > time_ms) ||
        abs(t - oldValue) > delta)
    {
      dpt_pwmLed.setValue(pwmLed);
      Serial.print(F("PWM LED:\t"));
      Serial.println(pwmLed);
      oldValue = t;            
      th = float2half(t);
      dpt_temperature.setValue(th);
      Serial.print(F("temperature:\t"));
      Serial.println(t);
      
      float h = random(100,10000)/100.0;
      hh = float2half(h);
      dpt_humidity.setValue(hh);
      Serial.print(F("humidity:\t"));
      Serial.println(h);

      unsigned int sensVal = analogRead(AIN);
      value = map(sensVal, 0, 1024, 0, 255);
      dpt_a2byte.setValue(value);
      Serial.print(F("Send Cyclic A2:\t"));
      Serial.println(value);

      if (i > 65535) i = 0;
      word value_word = i++;
      dpt_counter.setValue(value_word);
      Serial.print(F("pulse:\t\t"));
      Serial.println(value_word);

      sec=millis()/1000.0;
      dpt_uptime.setValue(sec);
      Serial.print(F("sec:\t\t"));
      Serial.println(sec);

      Serial.println();
      oldMillis = millis();
    }
  }
  
  uint16_t newValue = 0;
  if (knxIno.recive()) {
    dpt_pwmLed.getValue(pwmLed);
    analogWrite(LED, pwmLed);

    dpt_temperature.getValue(newValue);
    if ((newValue != 0) && (newValue != th)) {
      Serial.print(F("RX temperature:\t"));
      Serial.println(half2float(newValue));
    }
    newValue = 0;
    dpt_humidity.getValue(newValue);
    if ((newValue != 0) && (newValue != hh)) {
      Serial.print(F("RX humidity:\t"));
      Serial.println(half2float(newValue));
    }
    
    dpt_counter.getValue(i);
  }

  dpt_pwmLed.responseValue(pwmLed);
  dpt_temperature.responseValue(th);
  dpt_humidity.responseValue(hh);
  dpt_a2byte.responseValue(value);
  dpt_counter.responseValue(i);
  dpt_uptime.responseValue(sec);
}
