#ifndef ROTARYENCODER_H
#define ROTARYENCODER_H

#include <Arduino.h>

class RotaryEncoder {
public:
  RotaryEncoder(int clkPin, int dtPin, int swPin = -1);  // swPin optional
  long getCounter();             // Current position
  bool isButtonPressed();        // True on new press (edge)

private:
  int clkPin, dtPin, swPin;
  long counter = 0;
  int lastState = 0;
  bool lastSw = HIGH;
  bool swPressed = false;

  // State table for reliable decode (ignores invalid transitions)
  static const int8_t table[16];
};

#endif