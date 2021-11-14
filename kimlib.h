/*
    kimlib.h - Library for KNX module (KIM)
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

#ifndef KIMlib_h
#define KIMlib_h

#include <Arduino.h>

#define KIM_DEFAULT_ID    8     // Indirizzo I2C del Gateway KNX
#define KIM_BUFFER_LENGTH 96
#define KIM_WAIT          5
#define KIM_READY         0x83
#define KIM_READ_TIMEOUT  1000.0

// Messages al Module
enum MessageToKIM {
  VALUE_SEND          = 0x10,
  VALUE_READ_SEND     = 0x11,
  VALUE_RESPONSE_SEND = 0x12,
  PARAMETER_READ      = 0x41,
  SYS_PARAMETER_READ  = 0x61
};

// Messages dal Module
enum MessageFromKIM {
  VALUE_RECEIVE       = 0x20,
  VALUE_READ_RECEIVE  = 0x21,
  PARAMETER_RESPONSE  = 0x42,
  SYSTEM_EVENT        = 0x80
};

enum KIMeventID {
  SYS_EVENT           = 0x0100,
  CON_TRANSPOPT_LAYER = 0x0200,
  LOAD_STATE_MACHINE  = 0x0300,
  OBJ_OK              = 0x0402,
  OBJ_ERROR           = 0x0403,
  OBJ_NO              = 0x0410
};

static String printIdEvent(unsigned int mode)
{
  switch (mode) {
    case OBJ_OK:           
        return " confirmation – OK";
        break;
    case OBJ_ERROR:           
        return " confirmation – ERROR";
        break;
    case OBJ_NO:           
        return " no communication";
        break;
  } 
}

static void kimDataRedyHandler(); //prototype for interrupt handler

class KIMaip
{
  public:
	byte buf[KIM_BUFFER_LENGTH] = {};
	
    KIMaip(uint8_t pinDataReady, uint8_t pinKNXbus);
    
	boolean recive();
	boolean isReadBusy();
	void startReadTimeout();
	
	void setDevID(int x);
    int getDevID();

    void setIdDPTRead(int x);
    int getIdDPTRead();
  
    unsigned long getSystemEvent();
	String getVersion();
    word getKNXaddress();
    byte getProgMode();
    byte getI2Caddress();
    byte getI2CacutalAddress();
    unsigned long getSYSevent();
    byte getEnableError();
    unsigned int getI2Ctimeout();
    unsigned int getIndTimeout();
    unsigned int getMasterTimeout();
    byte getRtySend();
    byte getTransMode();
    byte getGroupLink();
    byte getDevStatus();
    byte getRXcommand();
    byte getRXdato();
    byte getRXvalue();
	
	void sendValue(int obj, byte nr);
	void sendResponseValue(int obj, byte nr);
	void sendReadValue(int obj);
	
    byte addUserParameter();
 
 protected:
	boolean isBusKNXready();
	void readSystemParameter(int ParId);	
	
 private: 
    byte _pinBusKNX = 0;
    byte _rxLength = 0;
    int _idDev = KIM_DEFAULT_ID;
    byte _rxCommand = 0;
    int _rxDato = 0;
    int _rxValue = 0;
    byte _idxUserParameter = 0;
    int _idDPTRead = -1;
    long _readTimeout = 0;
};

  /* *************************************  Legge i parametri di sistema   *************************************************************************
   *  
   *  Comando per leggere i parametri del sistema
   *  
   *************************************************************************************************************************************************  */
enum CmdKIM {
  KIM_VERSION        = 0x0001,  // Version
  KIM_KNX_ADDRESS    = 0x0002,  // Physical address
  KIM_PROG_MOD       = 0x0003,  // Programming mode
  KIM_I2C_ADDRESS    = 0x0005,  // I²C-Address, see Chap. 7.8.3.1
  KIM_ACTUAL_I2C     = 0x0006,  // Actual used I²C-Address, see Chap. 7.8.3.1
  KIM_SYS_EVENT      = 0x0011,  // Bit field to enable the System-Events
  KIM_EN_ERROR       = 0x0012,  // Enable error-messages
  KIM_I2C_TIMEOUT    = 0x0020,  // Timeout over an I²C-message
  KIM_IND_TIMEOUT    = 0x0021,  // Timeout for indications
  KIM_MASTER_TIMEOUT = 0x0022,  // Timeout for indications if no communication to the master
  KIM_RTY_SEND       = 0x0023,  // Retry sending indications, till no communication is detected
  KIM_TRANS_MOD      = 0x0100,  // Settings for transparent-mode
  KIM_GROUP_LINK     = 0x0110,  // GroupLink disabled
  KIM_DEV_STATUS     = 0x0200,  // State of Device, see Chap. 7.8.3.2
  KIM_DEV_RESET      = 0xF0F0   // Reset the module
};

//template<class T>
class UserParameter
{
	public:
		UserParameter(KIMaip *kimid);
		void setValue(uint8_t x);
		uint8_t getValue();
		int index();
    
	protected:
		void readParameter(int address, byte nr);
		
	private:
		uint8_t _idx;
		uint8_t _value;
		KIMaip *_kimaid;
};

class DPT
{
	public:
		DPT(uint8_t x, KIMaip *kimid)
		{
			_id = x;
			_kimaid = kimid;
		}
    
		template<class T>
		void setValue(T& value)
		{
			const byte* p = (const byte*)(const void*)&value;
			uint8_t len = sizeof(T);
			int y = len - 1;
			for (int i=0; i < len; i++) {
				_kimaid->buf[i] = ((byte *) p)[y--];
			}
			_kimaid->sendValue(_id, len);
		}
		template<class T>
		void getValue(T& value)
		{
            byte* p = (byte*)(void*)&value;
            uint8_t len = sizeof(T);
            int y = len + 1;
            if (_kimaid->getRXdato() == _id) {
                switch (_kimaid->getRXcommand()) {
                  case VALUE_RECEIVE:
                    if (_id == _kimaid->getIdDPTRead()) _kimaid->setIdDPTRead(-1);
                    for (int i=0; i < len; i++) {
				  *p++ = _kimaid->buf[y--];
				}
				break;
			}        
		  }
		}
    
		template<class T>
		void responseValue(T& value)
		{
			const byte* p = (const byte*)(const void*)&value;
			if (_kimaid->getRXdato() == _id) {   
				switch (_kimaid->getRXcommand()) {
				  case VALUE_READ_RECEIVE:
					uint8_t len = sizeof(T);
					int y = len - 1;
					for (int i=0; i < len; i++) {
						_kimaid->buf[i] = ((byte *) p)[y--];
					}
					_kimaid->sendResponseValue(_id, sizeof(T)); 
					break;
				}
			}
		}
		
		void getStatusValue()
		{			
		  if (!_kimaid->isReadBusy()) {    
			_kimaid->sendReadValue(_id);
			_kimaid->setIdDPTRead(_id);
			_kimaid->startReadTimeout();
		  }
		}
		
	private:
		byte _id;
		KIMaip *_kimaid;
};
#endif
