#include "ToggleSwitch.h"
#include <Arduino.h>

ToggleSwitch::ToggleSwitch(int pin) : pin(pin) {
  pinMode(pin, INPUT_PULLUP);
}

bool ToggleSwitch::isOn() {
  bool current = (digitalRead(pin) == LOW);
  if (current != lastState) {
    delay(50);  // Debounce
    current = (digitalRead(pin) == LOW);
    lastState = current;
  }
  return current;
}