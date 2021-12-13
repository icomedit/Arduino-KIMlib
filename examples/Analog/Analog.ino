/*
    Analog.ino - Example for KNX module (KIM) library

    Send to KNX BUS the follow date:
        - temperature (9.001) and humidity (9.007) read form AM232X sensor;
        - analog input in a byte (5.005);
        - counter in 2 bytes (7.001);
        - uptime in 4 bytes (14.074).

    Recive to KNX BUS value in PWM (5.001) for drive a led.

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
byte pwmLed          = 0;
unsigned long time_ms = 5000;
unsigned long old_millis = 0;
float sec = 0;
word i = 0;
uint16_t th = 0;
uint16_t hh = 0;
byte value = 0;

void setup() {
  Serial.begin(SERIAL_BIT_RATE);
  pinMode(LED, OUTPUT);
  Serial.println(F("\r"));
}

void loop() {

 if (time_ms != 0) {
    if ((millis() - old_millis) > time_ms) {

      dpt_pwmLed.setValue(pwmLed);
      Serial.print(F("PWM LED:\t"));
      Serial.println(pwmLed);

      //float t = AM2322.getTemperature();
      //th = float2half(t);
      dpt_temperature.setValue(th);
      Serial.print(F("temperature:\t"));
      //Serial.println(t);

      //float h = AM2322.getHumidity();
      //hh = float2half(h);
      dpt_humidity.setValue(hh);
      Serial.print(F("humidity:\t"));
      //Serial.println(h);

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
      old_millis = millis();
    }
  }

  if (knxIno.recive()) {
    dpt_pwmLed.getValue(pwmLed);
    analogWrite(LED, pwmLed);

    dpt_counter.getValue(i);
  }

  dpt_pwmLed.responseValue(pwmLed);
  dpt_temperature.responseValue(th);
  dpt_humidity.responseValue(hh);
  dpt_a2byte.responseValue(value);
  dpt_counter.responseValue(i);
  dpt_uptime.responseValue(sec);
}
