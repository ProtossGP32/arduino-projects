/*
  IR Relay controller:
  - Allows to control a 4-relay module
  - Allowed IR Protocols: NEC & SONY
  - SONY is filtered to only accept one input when the button is held down
*/
// Serial debugging
#define DEBUG false
#define DEBUG_SERIAL if(DEBUG)Serial
// Combo Decoder: Include only expected protocols to decode: NEC, SONY
#include <IRLibDecodeBase.h>
#include <IRLib_P01_NEC.h>
#include <IRLib_P02_Sony.h>
#include <IRLibCombo.h>
// Combo Decoder: Include the receiver class
#include <IRLibRecv.h>
// Include all known IR codes from available remote controllers
#include "IRCodes.h"

// Assign pins
#define IR_RECEIVER 11
#define RELAY_1 5
#define RELAY_1_NAME F("Relay 1")
#define RELAY_2 6
#define RELAY_2_NAME F("Relay 2")
#define RELAY_3 7
#define RELAY_3_NAME F("Relay 3")
#define RELAY_4 8
#define RELAY_4_NAME F("Relay 4")
#define RELAY_ALL 99
#define RELAY_ALL_NAME F("All relays")
// Define other useful strings
#define HYPHEN F(" - ")
#define OFF F(" off")
#define ON F(" on")

// Define the array of relays to control
int relays[] = { RELAY_1, RELAY_2, RELAY_3, RELAY_4 };

// Initialize receiver and decoder objects
IRrecv myReceiver(IR_RECEIVER);  //receiver on pin 5
IRdecode myDecoder;  //Decoder object

// IR Variables
#define SONY_NUM_REPEATS 0
#define SONY_RESTART_TIME 100
byte sonyCount = 0;
uint32_t previousSonyInput = 0xFFF;
unsigned long currentTime;
unsigned long lastInput;
const __FlashStringHelper* protocolName;
/*
relayAction: Enables or disables a relay based on its current state
*/
void relayAction(int relay, const __FlashStringHelper* relayName){
  // Print the protocol name
  DEBUG_SERIAL.print(protocolName);
  DEBUG_SERIAL.print(HYPHEN);
  DEBUG_SERIAL.print(relayName);
  // Sending RELAY_ALL will shutdown all relays at once
  if (relay == RELAY_ALL)
  {
    for (auto &&i : relays)
    {
      digitalWrite(i, HIGH);
    }
    DEBUG_SERIAL.println(OFF);
  }
  else {
    if (digitalRead(relay) == HIGH)
    {
      digitalWrite(relay, LOW);
      DEBUG_SERIAL.println(ON);
    }
    else
    {
      digitalWrite(relay, HIGH);
      DEBUG_SERIAL.println(OFF);
    }
  }
}
/*
irRemoteUpdate: Transforms an IR input into an action
- Right now, numbers from 0 to 4 control relays
- Compatible with NEC and SONY remote controllers
*/
void irRemoteUpdate(){
  if (myReceiver.getResults()) {
    myDecoder.decode();
    // Get the lastInput time, required for SONY restart counter
    lastInput = millis();
    // IR remote actions: Only allow SONY input on its SONY_NUM_REPEATS repetition
    if ( myDecoder.protocolNum == NEC || (myDecoder.protocolNum == SONY && sonyCount == SONY_NUM_REPEATS) ){
      protocolName = Pnames(myDecoder.protocolNum);
      switch(myDecoder.value) {
        case REPEAT_CODE: //Repeated code of previous press, do nothing
          break;
        case NEC_0:  //Number 0 (Stop all relays)
        case SONY_0:
          relayAction(RELAY_ALL, RELAY_ALL_NAME);
          break;
        case NEC_1:  //Number 1 (Relay 1)
        case SONY_1:
          relayAction(RELAY_1, RELAY_1_NAME);
          break;
        case NEC_2:  //Number 2 (Relay 2)
        case SONY_2:
          relayAction(RELAY_2, RELAY_2_NAME);
          break;
        case NEC_3:  //Number 3 (Relay 3)
        case SONY_3:
          relayAction(RELAY_3, RELAY_3_NAME);
          break;
        case NEC_4:  //Number 4 (Relay 4)
        case SONY_4:
          relayAction(RELAY_4, RELAY_4_NAME);
          break;
        default:
          DEBUG_SERIAL.print(protocolName);
          DEBUG_SERIAL.print(HYPHEN);
          DEBUG_SERIAL.println(F("Unknown command"));
          break;
      }
    }
    // Safety measures for SONY protocol
    if ( myDecoder.protocolNum == SONY ) {
      // Reset the SONY counter if a different button is pressed
      if (previousSonyInput != myDecoder.value){
        //DEBUG_SERIAL.println("Different SONY input, resetting counter");
        previousSonyInput = myDecoder.value;
        sonyCount = 0;
      }
      // Increase the SONY counter
      sonyCount += 1;
    }
    myReceiver.enableIRIn();  //Restart the receiver
  }
  else {
    currentTime = millis();
    // Restart the counter if no new inputs have been received in the time windows defined by SONY_RESTART_TIME
    if ( sonyCount != 0  && (currentTime - lastInput > SONY_RESTART_TIME) ) {
      sonyCount = 0;
    }
  }
}

void setup() {
  // Initialize current time
  lastInput = millis();
  DEBUG_SERIAL.begin(9600);
  // Initialize relays status
  for (auto &&i : relays)
  {
    digitalWrite(i, HIGH);
    pinMode(i, OUTPUT);
  }
  DEBUG_SERIAL.print(RELAY_ALL_NAME);
  DEBUG_SERIAL.print(HYPHEN);
  DEBUG_SERIAL.println(OFF);
  // TODO: Secure the first IR received signal, it is always treated as garbage
  myReceiver.enableIRIn(); // Start the receiver
  DEBUG_SERIAL.println(F("Ready to receive IR signals"));
}

void loop() {
  // IR receiver information retrieving
  irRemoteUpdate();
}
