#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

#include <Arduino.h>

class Potentiometer {
public:
  explicit Potentiometer(int pin);

  void update();                                   // Read and process

  int getRaw() const;                              // Raw ADC value (0-4095)

  float getNormalized(bool applyDeadzone = true);  // -1.0 to 1.0 (with optional deadzone)
  float getNormalizedRaw();                        // -1.0 to 1.0 full precision (no deadzone)

  // Calibration
  void beginCalibration(unsigned long durationMs = 5000);
  bool updateCalibration();                        // Call during calibration
  void endCalibration();
  void printCalibration() const;
  void setMinMaxCenter(int minV, int maxV, int centerV);

  void setDeadzone(int dz) { deadzone = dz; }      // Change deadzone (default 100)

private:
  int pin;
  int rawValue = 0;

  int minVal = 0;
  int maxVal = 4095;
  int centerVal = 2048;
  int deadzone = 100;  // Default ±100

  bool calibrating = false;
  unsigned long calibStart = 0;
  unsigned long calibDuration = 5000;

  float mapFloat(int val, int inMin, int inMax, float outMin, float outMax) const;
};

#endif