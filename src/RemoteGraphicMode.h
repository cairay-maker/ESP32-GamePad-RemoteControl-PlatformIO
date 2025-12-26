#ifndef REMOTEGRAPHICMODE_H
#define REMOTEGRAPHICMODE_H

#include "Mode.h"
#include "Hardware.h"

class RemoteGraphicMode : public Mode {
public:
  explicit RemoteGraphicMode(TFTHandler& tft, Hardware& hw);
  void enter() override;
  void update() override;
  void exit() override;

private:
  // References to shared hardware (owned by main.cpp Hardware instance)
  Joystick& joyRight;
  Joystick& joyLeft;
  AnalogKeyboard& keyboard;
  RotaryEncoder& encoderRight;
  RotaryEncoder& encoderLeft;
  RotaryEncoder& encoderMiddle;
  Potentiometer& potLeft;
  Potentiometer& potMid;
  Potentiometer& potRight;
  IMUReader& imu;
};

#endif
