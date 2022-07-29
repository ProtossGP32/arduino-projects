#include <Arduino.h>
#include <Keypad.h>
#include "Keyboard.h"

// Include Control_Surface library
#include <Control_Surface.h>

// Serial debugging
#define DEBUG_MACRO false
#define DEBUG_SERIAL if(DEBUG_MACRO)Serial

// Macropad select mode
const int SELECT_MODE_PIN = 9;
bool CONTROL_SURFACE = false;

// Control surface variables
// Interface type to use
USBMIDI_Interface midi;

// The note numbers corresponding to the buttons in the matrix
const AddressMatrix<4, 3> addresses = {{
  {1, 2, 3},
  {4, 5, 6},
  {7, 8, 9},
  {10, 11, 12},
}};
 
NoteButtonMatrix<4, 3> buttonmatrix = {
  {2, 3, 4, 5}, // row pins
  {6, 7, 8},    // column pins
  addresses,    // address matrix
  CHANNEL_1,    // channel and cable number
};

// Define pots {Pin number, Controller number}
CCPotentiometer potentiometers[] = {
  {A0, 0x10},
  {A1, 0x11},
  {A2, 0x12},
  {A3, 0x13},
};

// MacroPad as keyboard variables
const int ROW_NUM = 3; //four rows
const int COLUMN_NUM = 4; //three columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'7','8','9','*'},
  {'4','5','6','#'},
  {'1','2','3','0'}
};

byte pin_rows[ROW_NUM] = {8, 7, 6}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

uint8_t modifiers[3] = {NO_KEY, NO_KEY, NO_KEY};
int sizeOfModifiers = 0;
uint8_t keyToSend = NO_KEY;

void sendMacroCommand(uint8_t modifiers[], int sizeOfModifiers, uint8_t key)
{
  if (key != NO_KEY) {
    for (int k; k < sizeOfModifiers; k++)
    {
      Keyboard.press(modifiers[k]);
    }
    Keyboard.press(key);
    Keyboard.releaseAll();
  }
}

void setup(){
  // Initialize select mode pin
  pinMode(SELECT_MODE_PIN, INPUT_PULLUP);
  delay(1000);
  // Debugging purposes
  DEBUG_SERIAL.begin(9600);

  if (digitalRead(SELECT_MODE_PIN) == LOW) {
    DEBUG_SERIAL.println("Entering keyboard mode...");
    Keyboard.begin();
  }
  else {
    DEBUG_SERIAL.println("Entering MIDI mode...");
    CONTROL_SURFACE = true;
    Control_Surface.begin();
  }
}

void loop(){
  if (CONTROL_SURFACE == true) {
    // Loop over Control Surface
    Control_Surface.loop();
  }
  else {
    // Use KeyPad and Keyboard libraries to send key presses
    char key = keypad.getKey();

    if (key){
      // Bottom row
      // - Keypad 1 = CTRL + C (Copy)
      // - Keypad 2 = CTRL + V (Paste)
      // - Keypad 3 = CTRL + X (Cut)
      // - Keypad 0 = (Missing)
      // Middle row
      // - Keypad 4 = CTRL + Z (Undo)
      // - Keypad 5 = CTRL + S (Save)
      // - Keypad 6 = ALT + TAB (Change window)
      // - Keypad # = (Missing)
      // Top row
      // - Keypad 7 = (Pending) Mute/Unmute
      // - Keypad 8 = (Pending) Not defined yet
      // - Keypad 9 = (Missing)
      // - Keypad * = (Missing)
      DEBUG_SERIAL.println(key);
      switch(key) {
        case '1': // Copy
          modifiers[0] = {KEY_LEFT_CTRL};
          sizeOfModifiers = 1;
          keyToSend = 'c';
          break;
        case '2': // Paste
          modifiers[0] = {KEY_LEFT_CTRL};
          sizeOfModifiers = 1;
          keyToSend = 'v';
          break;
        case '3': // Cut
          modifiers[0] = {KEY_LEFT_CTRL};
          sizeOfModifiers = 1;
          keyToSend = 'x';
          break;
        case '4': // Undo
          modifiers[0] = {KEY_LEFT_CTRL};
          sizeOfModifiers = 1;
          keyToSend = 'z';
          break;
        case '5': // Save
          modifiers[0] = {KEY_LEFT_CTRL};
          sizeOfModifiers = 1;
          keyToSend = 's';
          break;
        case '6': // Change windows
          modifiers[0] = {KEY_LEFT_ALT};
          sizeOfModifiers = 1;
          keyToSend = KEY_TAB;
          break;
        case '7': // Test button
          sizeOfModifiers = 0;
          keyToSend = key;
          break;
        // Pending to add cases 7, 8, 9, #, * and 0
        default:
          sizeOfModifiers = 0;
          keyToSend = NO_KEY;
          break;
      }
    }
    else {
      sizeOfModifiers = 0;
      keyToSend = NO_KEY;
    }
    sendMacroCommand(modifiers, sizeOfModifiers, keyToSend);
  }
}