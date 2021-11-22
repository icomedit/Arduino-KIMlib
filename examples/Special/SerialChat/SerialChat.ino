/*
  Serial Chat example

  When new serial data arrives, this sketchadds it send data to KNX bus.
  When a newline is received, the loop prints the string and clears it.

  created 22 Nov 2021
  by Fabio Di Michele

  This example code is in the public domain.

*/

#include <kimlib.h>
#include "revers.h"

#define KNX_DATAREADY     2      // Pin data ready KNX
#define KNX_BUS           12     // Pin BUS KNX OK

#define SERIAL_BIT_RATE   115200 // Velocità della seriale
#define TEXT_LEN          14

// Object definition scope in ETS exacly sequnce respect
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
  Serial.println(F("\nText to send:")); 
}

void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {    
    inputString.getBytes(bufTX, TEXT_LEN + 1);  
    Serial.print(F("<TX:"));    

    revereseArray(bufTX, TEXT_LEN);
    memcpy (knxIno.buf, bufTX, TEXT_LEN); 
    printArray(knxIno.buf, TEXT_LEN);
    knxIno.sendValue(OBJ_TEXT, TEXT_LEN);    
    Serial.print(F("|"));  
    cleanArray(bufTX, TEXT_LEN); 
    Serial.println(inputString);  

    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  if (knxIno.recive()) { 
    systemEvent(knxIno.getSystemEvent());
    delay(10);
    Serial.print(F("RX>:"));
    dpt_text.getValue(bufRX);
    printArray(bufRX, TEXT_LEN);    
    Serial.print(F("|")); 
    for (int i = TEXT_LEN; i >= 0; i--) {
      Serial.print(bufRX[i]);
    }
    Serial.println();
  }
  dpt_text.responseValue(bufRX);
}

static String printObj(unsigned int id)
{
  String str = String(id);;
  switch (id) {
    case OBJ_TEXT:           
        str = F("Text");
        break;
  }
  return str; 
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

  Serial.print(F("Sytem Event: 0x"));
  Serial.println(l.temp_long, HEX);
//  Serial.println(printObj(objN));
//  Serial.println(printIdEvent(idEvent));
}

static String printIdEvent(unsigned int mode)
{
  String str = String(mode);
  switch (mode) {
    case OBJ_OK:           
        str = F(" confirmation – OK");
        break;
    case OBJ_ERROR:           
        str = F(" confirmation – ERROR");
        break;
    case OBJ_NO:           
        str = F(" no communication");
        break;
  } 
  return str;
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
