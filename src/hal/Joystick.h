#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <Arduino.h>

class Joystick {
public:
  // Constructor with optional custom center (default 2048)
  Joystick(int pinX, int pinY, int centerX = 2048, int centerY = 2048, int deadzone = 100);

  void update();                    // Read and process both axes

  // 1. Raw values (direct from ADC)
  int getRawX() const { return rawX; }
  int getRawY() const { return rawY; }

  // 2. Deadzoned values (centered, deadzone applied)
  int getDeadzonedX() const { return deadzonedX; }
  int getDeadzonedY() const { return deadzonedY; }

  // 3. Mapped values (-1.0 to 1.0 float)
  float getMappedX() const { return mappedX; }
  float getMappedY() const { return mappedY; }

private:
  int pinX, pinY;
  int centerX, centerY;
  int deadzone;

  int rawX = 0, rawY = 0;
  int deadzonedX = 0, deadzonedY = 0;
  float mappedX = 0.0f, mappedY = 0.0f;

  static constexpr int ADC_MAX = 4095;
};

#endif