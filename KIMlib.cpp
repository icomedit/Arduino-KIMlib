/*
    kimlib.cpp - Library for KNX module (KIM)
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

#include <Arduino.h>
#include <KIMlib.h>
#include <Wire.h>

//**************************** Routine di interrupt su fronte di salita del dato disponibile ******************************
volatile boolean kimBufFull = false;

static void kimDataRedyHandler()
{
  kimBufFull = true;
}

// CONTRUCTORS
KIMaip::KIMaip(uint8_t pinDataReady, uint8_t pinKNXbus)
{
  Wire.begin();                        // Inizializza I2C
  pinMode(pinKNXbus, INPUT_PULLUP);    // Pin 14 Gateway KNX
  pinMode(pinDataReady, INPUT_PULLUP); // Pin 1 Gateway KNX
  attachInterrupt(digitalPinToInterrupt(pinDataReady), kimDataRedyHandler, RISING);        // abilitazione funzione interrupt su fronte di salita pin 2
  _pinBusKNX = pinKNXbus;
}

void KIMaip::setDevID(int x)
{
  _idDev = x;
}

int KIMaip::getDevID()
{
  return _idDev;
}

boolean KIMaip::isReadBusy()
{
  if (_idDPTRead != -1) return true;
  else return false;
}

void KIMaip::setIdDPTRead(int x)
{
  _idDPTRead = x;
}

void KIMaip::startReadTimeout()
{
  _readTimeout = millis();
}

int KIMaip::getIdDPTRead()
{
  return  _idDPTRead;
}

boolean KIMaip::recive()
{
  unsigned long stopWatch = millis();

  if ((millis() - _readTimeout) >  KIM_READ_TIMEOUT) _idDPTRead = -1;

  if (this->isBusKNXready())
  {
	while( (millis() - stopWatch) < 50)
	{ // Timeout di attesa risposta
	  if (kimBufFull)
	  {
		Wire.requestFrom(_idDev, 40);      // request 40 bytes from slave device DevId
		_rxLength = Wire.read();           // receive Length
		_rxCommand = Wire.read();              // receive SendCommand
		for (int i=0; i < (_rxLength-(1)); i++)
		{
		  buf[i] = Wire.read();        // receive next byte of telegram and load Array Buffer
		}
		_rxDato = int(buf[0]*256) + int(buf[1]);   // Azioni per il valore ricevuto di ciascun oggetto
		_rxValue = int(buf[2]*256) + int(buf[3]);
		kimBufFull = false;       // Reset Flag
		return true;
	  }
	}
	return false;
  }
}

unsigned long KIMaip::getSystemEvent()
{
  union l_tag {                           // Siccome la temperatura è un dato FLOAT, si usa la funzione Union per "impacchettare" i 4 BYTE che la compongono.
	unsigned long temp_long;                // Se copi dentro "u.temp_float" un valore float automaticamente ottieni i relativi quattro byte nel array "u.temp_byte[n]", con n compreso tra 0 e 3,
	byte temp_byte[4];                      // Viceversa se copy nel array i quattro byte, ottieni il tuo valore float dentro u.temp_float.
  } l;
  int y = 3;
  if (_rxCommand == SYSTEM_EVENT) {
	for (int i=0; i < 4; i++){
	  l.temp_byte[y--] = buf[i];
	}
  }
  return l.temp_long;
}

String KIMaip::getVersion()
{
  String str = "";
  this->readSystemParameter(KIM_VERSION);
  delay(KIM_WAIT);
  if (this->recive()) {
	if (_rxDato == KIM_VERSION) {
	  for (int i=0; i < (_rxLength-3); i++){
		if (i > 0) str += F(".");
		str += buf[i+2];
	  }
	}
  }
  return str;
}

word KIMaip::getKNXaddress()
{
  union w_tag {                           // Siccome la temperatura è un dato FLOAT, si usa la funzione Union per "impacchettare" i 4 BYTE che la compongono.
	word temp_word;                         // Se copi dentro "u.temp_float" un valore float automaticamente ottieni i relativi quattro byte nel array "u.temp_byte[n]", con n compreso tra 0 e 3,
	byte temp_byte[2];                      // Viceversa se copy nel array i quattro byte, ottieni il tuo valore float dentro u.temp_float.
  } w;
  int len = _rxLength - 3;
  this->readSystemParameter(KIM_KNX_ADDRESS);
  delay(KIM_WAIT);
  if (this->recive()) {
	if (_rxDato == KIM_KNX_ADDRESS) {
	  for (int i=0; i < len; i++){
		if (i < 2) w.temp_byte[i] = buf[i+2];
	  }
	}
  }
  return w.temp_word;
}

byte KIMaip::getProgMode()
{
  this->readSystemParameter(KIM_PROG_MOD);
  delay(KIM_WAIT);
  if (this->recive()) {
	if (_rxDato == KIM_PROG_MOD) {
	  return buf[2];
	}
  }
  return 0;
}

byte KIMaip::getI2Caddress()
{
  this->readSystemParameter(KIM_I2C_ADDRESS);
  delay(KIM_WAIT);
  if (this->recive()) {
	if (_rxDato == KIM_I2C_ADDRESS) {
	  return buf[2];
	}
  }
  return 0;
}

byte KIMaip::getI2CacutalAddress()
{
  this->readSystemParameter(KIM_ACTUAL_I2C);
  delay(KIM_WAIT);
  if (this->recive()) {
	if (_rxDato == KIM_ACTUAL_I2C) {
	  return buf[2];
	}
  }
  return 0;
}

unsigned long KIMaip::getSYSevent()
{
  union l_tag {                           // Siccome la temperatura è un dato FLOAT, si usa la funzione Union per "impacchettare" i 4 BYTE che la compongono.
	unsigned long temp_long;                // Se copi dentro "u.temp_float" un valore float automaticamente ottieni i relativi quattro byte nel array "u.temp_byte[n]", con n compreso tra 0 e 3,
	byte temp_byte[4];                      // Viceversa se copy nel array i quattro byte, ottieni il tuo valore float dentro u.temp_float.
  } l;
  int len = _rxLength - 3;
  this->readSystemParameter(KIM_SYS_EVENT);
  delay(KIM_WAIT);
  if (this->recive()) {
	if (_rxDato == KIM_SYS_EVENT) {
	  for (int i=0; i < len; i++){
		if (i < 4) l.temp_byte[i] = buf[i+2];
	  }
	}
  }
  return l.temp_long;
}

byte KIMaip::getEnableError()
{
  this->readSystemParameter(KIM_EN_ERROR);
  delay(KIM_WAIT);
  if (this->recive()) {
	if (_rxDato == KIM_EN_ERROR) {
	  return buf[2];
	}
  }
  return 0;
}

unsigned int KIMaip::getI2Ctimeout()
{
  this->readSystemParameter(KIM_I2C_TIMEOUT);
  delay(KIM_WAIT);
  if (this->recive()) {
	if (_rxDato == KIM_I2C_TIMEOUT) {
	  return _rxValue;
	}
  }
  return 0;
}

unsigned int KIMaip::getIndTimeout()
{
  this->readSystemParameter(KIM_IND_TIMEOUT);
  delay(KIM_WAIT);
  if (this->recive()) {
	if (_rxDato == KIM_IND_TIMEOUT) {
	  return _rxValue;
	}
  }
  return 0;
}

unsigned int KIMaip::getMasterTimeout()
{
  this->readSystemParameter(KIM_MASTER_TIMEOUT);
  delay(KIM_WAIT);
  if (this->recive()) {
	if (_rxDato == KIM_MASTER_TIMEOUT) {
	  return _rxValue;
	}
  }
  return 0;
}

byte KIMaip::getRtySend()
{
  this->readSystemParameter(KIM_RTY_SEND);
  delay(KIM_WAIT);
  if (this->recive()) {
	if (_rxDato == KIM_RTY_SEND) {
	  return buf[2];
	}
  }
  return 0;
}

byte KIMaip::getTransMode()
{
  this->readSystemParameter(KIM_TRANS_MOD);
  delay(KIM_WAIT);
  if (this->recive()) {
	if (_rxDato == KIM_TRANS_MOD) {
	  return buf[2];
	}
  }
  return 0;
}

byte KIMaip::getGroupLink()
{
  this->readSystemParameter(KIM_GROUP_LINK);
  delay(KIM_WAIT);
  if (this->recive()) {
	if (_rxDato == KIM_GROUP_LINK) {
	  return buf[2];
	}
  }
  return 0;
}

byte KIMaip::getDevStatus()
{
  this->readSystemParameter(KIM_DEV_STATUS);
  delay(KIM_WAIT);
  if (this->recive()) {
	if (_rxDato == KIM_DEV_STATUS) {
	  return buf[2];
	}
  }
  return 0;
}

byte KIMaip::getRXcommand()
{
  return _rxCommand;
}

byte KIMaip::getRXdato()
{
  return _rxDato;
}

byte KIMaip::getRXvalue()
{
  return _rxValue;
}

byte KIMaip::addUserParameter()
{
  return _idxUserParameter++;
}

boolean KIMaip::isBusKNXready()
{
  return digitalRead(_pinBusKNX);
}

void KIMaip::readSystemParameter(int ParId)
{
  // step 1: instruct device to SYSTEM PARAMETER READ
  Wire.beginTransmission(_idDev); // transmit to device DevId (0x08)
  Wire.write(byte(0x03));         // sets Length (0x03)
  Wire.write(byte(SYS_PARAMETER_READ));         // sets Command for Syst Param Read: (0x61)
  Wire.write(highByte(ParId));    // Syst Param-IDS 1/2
  Wire.write(lowByte(ParId));     // Syst Param-IDS 2/2
  Wire.endTransmission();         // stop transmitting
  kimBufFull = false;             // Reset Flag
}

//**************************** DPT ******************************

/* *************************************  Invia un valore ad un oggetto   ***********************************************************************
*
*  Comando per inviare i valori dell'Buffer di dimensione nr all'oggetto obj
*
*************************************************************************************************************************************************  */
void KIMaip::sendValue(int obj, byte nr) {

	// step 1: instruct device to OBJECT SEND VALUE
	Wire.beginTransmission(_idDev);  // transmit to device DevId (0x08)
	Wire.write(byte(0x03+nr));      // sets Length (0x03) + nr
	Wire.write(byte(VALUE_SEND));   // sets Command for Send Value: (0x10)
	Wire.write(highByte(obj));      // Obj-IDS 1/2
	Wire.write(lowByte(obj));       // Obj-IDS 2/2
	for (int i=0; i < nr; i++){
	  Wire.write(byte(buf[i]));     // Object value
	}
	Wire.endTransmission();         // stop transmitting
	kimBufFull = false;             // Reset Flag
}

/* *************************************  Risponde ad una richiesta di lettura da parte dal BUS KNX verso un oggetto  ***************************
*
*  Comando per inviare i valori dell'Buffer di dimensione nr all'oggetto obj in risposta ad una richiesta di lettura pervenuta dal BUS KNX
*
*************************************************************************************************************************************************  */
void KIMaip::sendResponseValue(int obj, byte nr) {

	// step 1: instruct device to OBJECT SEND RESPONSE VALUE
	Wire.beginTransmission(_idDev); // transmit to device DevId (0x08)
	Wire.write(byte(0x03+nr));      // sets Length (0x03) + nr
	Wire.write(byte(VALUE_RESPONSE_SEND));         // sets Command for Send Value: (0x12)
	Wire.write(highByte(obj));      // Obj-IDS 1/2
	Wire.write(lowByte(obj));       // Obj-IDS 2/2
	for (int i=0; i < nr; i++){
	  Wire.write(byte(buf[i]));     // Object value
	}
	Wire.endTransmission();         // stop transmitting
	kimBufFull = false;             // Reset Flag
}

/* *************************************  Invia una richiesta di leggere un valore dal BUS KNX ad un oggetto   ***********************************
*
*  Comando per inviare la richiesta di lettura dal BUS KNX all'oggetto obj
*  Il dispositvo sul BUS KNX che è abilitato a rispondere, invierà il valore all'indirizzo di gruppo deve è contenuto obj
*
*************************************************************************************************************************************************  */
void KIMaip::sendReadValue(int obj) {

	// step 1: instruct device to OBJECT SEND READ VALUE
	Wire.beginTransmission(_idDev); // transmit to device DevId (0x08)
	Wire.write(byte(0x03));         // sets Length (0x03)
	Wire.write(byte(VALUE_READ_SEND));         // sets Command for SendReadValue: (0x11)
	Wire.write(highByte(obj));      // Obj-IDS 1/2
	Wire.write(lowByte(obj));       // Obj-IDS 2/2
	Wire.endTransmission();         // stop transmitting
	kimBufFull = false;             // Reset Flag
}

// *************************************  User Parameter   ***********************************
// CONTRUCTORS
UserParameter::UserParameter(KIMaip *kimid)
{
  _value = 0;
  _idx = kimid->addUserParameter();
  //_idx = id;
  _kimaid = kimid;
}

void UserParameter::setValue(uint8_t x)
{
  _value = x;
}

uint8_t UserParameter::getValue()
{
  this->readParameter(_idx, 1);
  delay(KIM_WAIT);
  if (_kimaid->recive()) {
	if (_kimaid->getRXcommand() == PARAMETER_RESPONSE) {
		_value = _kimaid->buf[3];
	}
  }
  return _value;
}

int UserParameter::index()
{
  return _idx;
}

/* *************************************  Legge i parametri   ***********************************************************************************
 *
 *  Comando per leggere nr parametri (max 96 bytes) a partire dall'indirizzo address
 *
 *************************************************************************************************************************************************  */

void UserParameter::readParameter(int address, byte nr) {

	// step 1: instruct device to  PARAMETER READ
	Wire.beginTransmission(_kimaid->getDevID());  // transmit to device DevId (0x08)
	Wire.write(byte(0x04));             // sets Length (0x04)
	Wire.write(byte(PARAMETER_READ));   // sets Command for Param Read: (0x41)
	Wire.write(highByte(address));      // Param-IDS 1/2
	Wire.write(lowByte(address));       // Param-IDS 2/2
	Wire.write(nr);                     // Nr Param
	Wire.endTransmission();             // stop transmitting
	kimBufFull = false;                 // Reset Flag
}

// Functions
union u_tag {
    uint16_t temp;
    byte temp_byte[2] ;
}u;

uint16_t float2half (float f) {
  float v = f * 100.0f;
  int exponent = 0;
  for (; v < -2048.0f; v /= 2) exponent++;
  for (; v > 2047.0f; v /= 2) exponent++;
  long m = round(v) & 0x7FF;
  short msb = (short) (exponent << 3 | m >> 8);
  if (f < 0.0f) msb |= 0x80;
  u.temp_byte[1] = msb;
  u.temp_byte[0] = (byte)m;
  return u.temp;
}

float half2float (uint16_t halfFloat) {
  u.temp = halfFloat;
  int exponent = (u.temp_byte[0] & B01111000) >> 3;
  int mantissa = ((u.temp_byte[0] & B00000111) << 8) | u.temp_byte[1];

  if(u.temp_byte[0] & B10000000) {
    return ((-2048 + mantissa) * 0.01) * pow(2.0, exponent);
  }
  return (mantissa * 0.01) * pow(2.0, exponent);
}
