#ifndef REMOTECONTROLMODE_H
#define REMOTECONTROLMODE_H

#include "Mode.h"
#include "Joystick.h"
#include "AnalogKeyboard.h"
#include "RotaryEncoder.h"
#include "Potentiometer.h"
#include "IMUReader.h"

class RemoteControlMode : public Mode {
public:
  // Constructor takes TFT reference and initializes all sensors
  explicit RemoteControlMode(TFTHandler& tft);

  void enter() override;
  void update() override;
  void exit() override;

private:
  // === All Input Hardware ===
  // Right Joystick - swapped pins + calibrated center
  Joystick joyRight{36, 39, 1920, 1925, 100};  
  // pinX=36 (logical X), pinY=39 (logical Y)
  // centerX=1920, centerY=1925
  // deadzone=100

  // Left Joystick - swapped pins + calibrated center
  Joystick joyLeft{13, 12, 1910, 1995, 100};
  // pinX=13 (logical X), pinY=12 (logical Y)
  // centerX=1910, centerY=1995
  // deadzone=100

  // No inversion needed (remove any true/true if you added them)
  // Just swap the order: Joystick(pinForLogicalX, pinForLogicalY)

  AnalogKeyboard keyboard{34};        // 5-button analog keyboard on GPIO34

  RotaryEncoder encoderRight{33, 32, 35};   // CLK, DT, SW
  RotaryEncoder encoderLeft{27, 26, 25};    // CLK, DT, SW (SW noted not working)

  Potentiometer potRight{14};         // Right pot
  Potentiometer potMid{4};            // Middle pot
  Potentiometer potLeft{15};          // Left pot

  IMUReader imu;                      // LSM6DS33 on SDA=21, SCL=22
};

#endif