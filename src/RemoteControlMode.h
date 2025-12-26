#ifndef REMOTECONTROLMODE_H
#define REMOTECONTROLMODE_H

#include "Mode.h"
#include "Hardware.h"

class RemoteControlMode : public Mode {
public:
  // Constructor takes TFT reference and shared hardware
  explicit RemoteControlMode(TFTHandler& tft, Hardware& hw);

  void enter() override;
  void update() override;
  void exit() override;

private:
  // === References to shared input hardware ===
  Joystick& joyRight;
  // pinX=36 (logical X), pinY=39 (logical Y)
  // centerX=1920, centerY=1925
  // deadzone=100

  // Left Joystick - swapped pins + calibrated center
  Joystick& joyLeft;
  // pinX=13 (logical X), pinY=12 (logical Y)
  // centerX=1910, centerY=1995
  // deadzone=100

  // No inversion needed (remove any true/true if you added them)
  // Just swap the order: Joystick(pinForLogicalX, pinForLogicalY)

  AnalogKeyboard& keyboard;        // 5-button analog keyboard on GPIO34

  RotaryEncoder& encoderRight;   // CLK, DT, SW
  RotaryEncoder& encoderLeft;    // CLK, DT, SW (SW noted not working)

  Potentiometer& potRight;         // Right pot
  Potentiometer& potMid;            // Middle pot
  Potentiometer& potLeft;          // Left pot

  IMUReader& imu;                      // LSM6DS33 on SDA=21, SCL=22
};

#endif