#include "AnalogKeyboard.h"

AnalogKeyboard::AnalogKeyboard(int pin) : pin(pin) {}

String AnalogKeyboard::getCurrentKey() {
  int val = analogRead(pin);
  if (val > 4000) return "LEFT";
  if (val > 3200 && val < 3700) return "UP";
  if (val > 2500 && val < 2900) return "DOWN";
  if (val > 1800 && val < 2150) return "RIGHT";
  if (val > 600 && val < 1000) return "SELECT";
  return "NONE";
}

// Logic: Just returns true if the key name matches. 
// We will handle the "Holding" timer in the Menu class for better stability.
bool AnalogKeyboard::isKeyHeld(String keyName) {
  return (getCurrentKey() == keyName);
}

String AnalogKeyboard::getPressedKey() {
  String current = getCurrentKey();
  String pressed = "NONE";

  // Only return the key on the initial transition from NONE to KEY
  if (current != "NONE" && lastKey == "NONE") {
    pressed = current;
  }

  lastKey = current;
  return pressed;
}