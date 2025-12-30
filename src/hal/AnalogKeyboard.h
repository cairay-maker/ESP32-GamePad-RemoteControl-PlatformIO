#ifndef ANALOGKEYBOARD_H
#define ANALOGKEYBOARD_H

#include <Arduino.h>

class AnalogKeyboard {
public:
  explicit AnalogKeyboard(int pin);
  String getCurrentKey();      
  String getPressedKey();      
  bool isKeyHeld(String keyName); 

private:
  int pin;
  String lastKey = "NONE";     
  unsigned long lastStartTime = 0; // Tracks when the current key started
  bool wasProcessed = false;       // Prevents "skipping" by locking the press
};

#endif