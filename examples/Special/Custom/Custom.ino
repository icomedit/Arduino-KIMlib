/*
    Custom.ino - Example for KNX module (KIM) library

    Send 10 bytes to KNX BUS for konnex custom value for simulate energy meter.
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
#include <EmonLib.h>             // Include Emon Library

EnergyMonitor emon1;             // Create an instance

#define KNX_DATAREADY     2      // Pin data ready KNX
#define KNX_BUS           12     // Status BUS KNX
#define EXT_TV            A2     // voltage sensor
#define EXT_TA            A3     // current sensor

#define RELAY             7      // Pin relay with lamp

#define OBJ_ENERGY        28

#define SERIAL_BIT_RATE   115200 // Serial monitor speed
#define TA_CALIBRATION    4.9    // SCT013 5A/1V
#define TV_CALIBRATION    137.03 // TV homemade
#define WAIT              60000

KIMaip knxIno(KNX_DATAREADY, KNX_BUS);
DPT dpt_energy(OBJ_ENERGY, &knxIno);

// variables will change:
unsigned long old_millis = 0;
boolean toggle = false;

struct energy_t
{
  uint16_t volt = 0;
  uint16_t curr = 0;
  uint16_t powerFactor = 0;
  uint16_t apparentPower = 0;
  uint16_t realPower = 0;
} energy;

void setup()
{
  Serial.begin(SERIAL_BIT_RATE);
  Serial.println(F("\r"));
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, toggle);
  emon1.current(EXT_TA, TA_CALIBRATION);     // Current: input pin, calibration.
  emon1.voltage(EXT_TV, TV_CALIBRATION, 1);  // Voltage: input pin, calibration, phase_shift
}

void loop()
{
  emon1.calcVI(20,2000);         // Calculate all. No.of half wavelengths (crossings), time-out
  emon1.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)

  energy.realPower       = float2half(emon1.realPower);        //extract Real Power into variable
  energy.apparentPower   = float2half(emon1.apparentPower);    //extract Apparent Power into variable
  energy.powerFactor     = float2half(emon1.powerFactor);      //extract Power Factor into Variable
  energy.volt            = float2half(emon1.Vrms);             //extract Vrms into Variable
  energy.curr            = float2half(emon1.Irms);             //extract Irms into Variable

  if ((millis() - old_millis) > WAIT) {
    dpt_energy.setValue(energy);
    toggle = !toggle;
    old_millis = millis();
  }
  digitalWrite(RELAY, toggle);

  dpt_energy.responseValue(energy);
}
