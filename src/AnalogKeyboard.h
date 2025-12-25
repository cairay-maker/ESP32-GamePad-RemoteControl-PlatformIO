#ifndef ANALOGKEYBOARD_H
#define ANALOGKEYBOARD_H

#include <Arduino.h>

class AnalogKeyboard {
public:
  explicit AnalogKeyboard(int pin);
  String getCurrentKey();      // ← Returns current key or "NONE"
  String getPressedKey();      // ← Returns key only on new press

private:
  int pin;
  String lastKey = "NONE";     // For edge detection
};

#endif