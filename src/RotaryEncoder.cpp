#include "RotaryEncoder.h"

// State transition table for robust rotary decoding
// Indexes: previous state (2 bits CLK/DT) << 2 | current state
const int8_t RotaryEncoder::table[16] = {
  0,  -1,  1,  0,   // 00xx
  1,   0,  0, -1,   // 01xx
  -1,   0,  0,  1,   // 10xx
  0,   1, -1,  0    // 11xx
};

RotaryEncoder::RotaryEncoder(int clk, int dt, int sw)
    : clkPin(clk), dtPin(dt), swPin(sw) {
  pinMode(clkPin, INPUT_PULLUP);
  pinMode(dtPin, INPUT_PULLUP);
  if (swPin != -1) pinMode(swPin, INPUT_PULLUP);

  // Read initial state
  lastState = (digitalRead(clkPin) << 1) | digitalRead(dtPin);
}

long RotaryEncoder::getCounter() {
  int currentState = (digitalRead(clkPin) << 1) | digitalRead(dtPin);
  if (currentState != lastState) {
    counter += table[(lastState << 2) | currentState];
    lastState = currentState;
  }
  return counter;
}

bool RotaryEncoder::isButtonPressed() {
  if (swPin == -1) return false;

  bool current = (digitalRead(swPin) == LOW);
  if (current && !lastSw) {
    delay(10);  // Simple debounce
    if (digitalRead(swPin) == LOW) {
      swPressed = true;
    }
  } else {
    swPressed = false;
  }
  lastSw = current;
  return swPressed;
}