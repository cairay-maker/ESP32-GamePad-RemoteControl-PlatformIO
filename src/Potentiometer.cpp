#include "Potentiometer.h"
#include <Arduino.h>

Potentiometer::Potentiometer(int pin)
    : pin(pin)
{
  update();  // Initial read
}

void Potentiometer::update() {
  rawValue = analogRead(pin);
}

int Potentiometer::getRaw() const {
  return rawValue;
}

float Potentiometer::getNormalized(bool applyDeadzone) {
  update();  // Ensure latest value

  int value = rawValue;

  if (applyDeadzone && abs(value - centerVal) < deadzone) {
    value = centerVal;
  }

  if (value <= centerVal) {
    if (value == centerVal) return 0.0f;
    return mapFloat(value, minVal, centerVal, -1.0f, 0.0f);
  } else {
    return mapFloat(value, centerVal, maxVal, 0.0f, 1.0f);
  }
}

float Potentiometer::getNormalizedRaw() {
  return getNormalized(false);  // Full precision, no deadzone
}

void Potentiometer::beginCalibration(unsigned long durationMs) {
  calibrating = true;
  calibStart = millis();
  calibDuration = durationMs;
  minVal = rawValue;
  maxVal = rawValue;
  Serial.println("Pot calibration started - move to extremes");
}

bool Potentiometer::updateCalibration() {
  if (!calibrating) return false;

  update();

  if (rawValue < minVal) minVal = rawValue;
  if (rawValue > maxVal) maxVal = rawValue;

  if (millis() - calibStart >= calibDuration) {
    centerVal = (minVal + maxVal) / 2;
    calibrating = false;
    printCalibration();
    return true;
  }
  return false;
}

void Potentiometer::endCalibration() {
  if (calibrating) {
    centerVal = (minVal + maxVal) / 2;
    calibrating = false;
    printCalibration();
  }
}

void Potentiometer::printCalibration() const {
  Serial.printf("Pot calibrated: min=%d, max=%d, center=%d, deadzone=±%d\n",
                minVal, maxVal, centerVal, deadzone);
}

void Potentiometer::setMinMaxCenter(int minV, int maxV, int centerV) {
  minVal = minV;
  maxVal = maxV;
  centerVal = centerV;
  calibrating = false;
}

float Potentiometer::mapFloat(int val, int inMin, int inMax, float outMin, float outMax) const {
  return outMin + (outMax - outMin) * (val - inMin) / float(inMax - inMin);
}