#ifndef ANALOGKEYBOARD_H
#define ANALOGKEYBOARD_H

#include <Arduino.h>

class AnalogKeyboard {
public:
  explicit AnalogKeyboard(int pin);
  String getCurrentKey();      
  String getPressedKey();      
  bool isKeyHeld(String keyName); // ← Add this to track duration

private:
  int pin;
  String lastKey = "NONE";     
};

#endif