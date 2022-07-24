#include <Arduino.h>
#include "Keyboard.h"

void setup() {
  // put your setup code here, to run once:
  // Initialize button pins
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  pinMode(15, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  // Initialize serial connection
  Serial.begin(9600);
  // Initialize keyboard
  Keyboard.begin();
}


// Pin 6 = CTRL + C (Copy)
// Pin 7 = CTRL + V (Paste)
// Pin 8 = CTRL + X (Cut)
// Pin 9 = CTRL + Z (Undo)
// Pin 10 = CTRL + S (Save)
// Pin 14 = ALT + TAB (Change window)
// Pin 15 = 
// Pin 16 =

// Last state of the buttons
int lastButtonState[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int macroMap[8] = {6, 7, 8, 9, 10, 14, 15, 16};
char key = '0';
uint8_t modifiers[3] = {'0', '0', '0'};
int sizeOfModifiers = 0;

void sendMacroCommand(uint8_t modifiers[], int sizeOfModifiers, uint8_t key)
{
  for (int k; k < sizeOfModifiers; k++)
  {
    Keyboard.press(modifiers[k]);
  }
  Keyboard.press(key);
}

void loop() {
  // put your main code here, to run repeatedly:

  for (int index = 0; index < 8; index++)
  {
    int currentButtonState = !digitalRead(macroMap[index]);
    if (currentButtonState != lastButtonState[index])
    {
      if (currentButtonState == 1) {
        // Update the macro command variables
        switch (index) {
          case 0: // Copy
            modifiers[0] = {KEY_LEFT_CTRL};
            sizeOfModifiers = 1;
            key = 'c';    
            break;
          case 1: // Paste
            modifiers[0] = {KEY_LEFT_CTRL};
            sizeOfModifiers = 1;
            key = 'v';
            break;
          case 2: // Cut
            modifiers[0] = {KEY_LEFT_CTRL};
            sizeOfModifiers = 1;
            key = 'x';
            break;
          case 3: // Undo
            modifiers[0] = {KEY_LEFT_CTRL};
            sizeOfModifiers = 1;
            key = 'z';
            break;
          case 4: // Save
            modifiers[0] = {KEY_LEFT_CTRL};
            sizeOfModifiers = 1;
            key = 's';
            break;
          case 5: // Change windows
            modifiers[0] = {KEY_LEFT_ALT};
            sizeOfModifiers = 1;
            key = KEY_TAB;
            break;
        }
        // Send the macro command
        sendMacroCommand(modifiers, sizeOfModifiers, key);
      }
      // Update the last button state value
      lastButtonState[index] = currentButtonState;
    }
  }
  // Give some delay to loop again
  delay(100);
  // this releases the buttons
  Keyboard.releaseAll(); 
}
