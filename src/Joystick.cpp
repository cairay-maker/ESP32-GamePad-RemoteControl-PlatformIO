#include "Joystick.h"

Joystick::Joystick(int pinX, int pinY, int centerX, int centerY, int deadzone)
    : pinX(pinX), pinY(pinY), centerX(centerX), centerY(centerY), deadzone(deadzone)
{
  // Initial read to avoid garbage
  update();
}

void Joystick::update() {
  rawX = analogRead(pinX);
  rawY = analogRead(pinY);

  // Apply deadzone
  int offsetX = rawX - centerX;
  int offsetY = rawY - centerY;

  if (abs(offsetX) < deadzone) offsetX = 0;
  if (abs(offsetY) < deadzone) offsetY = 0;

  // Map using actual observed range (replace with your values!)
  const int minX = 0;   // ← Your measured min X
  const int maxX = 4095;  // ← Your measured max X
  const int minY = 0;   // ← Your measured min Y
  const int maxY = 4095;  // ← Your measured max Y

  // Symmetric mapping to -1.0 to 1.0
  if (offsetX == 0) {
    mappedX = 0.0f;
  } else if (offsetX > 0) {
    mappedX = static_cast<float>(offsetX) / (maxX - centerX);
  } else {
    mappedX = static_cast<float>(offsetX) / (centerX - minX);
  }

  if (offsetY == 0) {
    mappedY = 0.0f;
  } else if (offsetY > 0) {
    mappedY = static_cast<float>(offsetY) / (maxY - centerY);
  } else {
    mappedY = static_cast<float>(offsetY) / (centerY - minY);
  }

  // Clamp just in case
  if (mappedX > 1.0f) mappedX = 1.0f;
  if (mappedX < -1.0f) mappedX = -1.0f;
  if (mappedY > 1.0f) mappedY = 1.0f;
  if (mappedY < -1.0f) mappedY = -1.0f;
}