#ifndef HARDWARE_H
#define HARDWARE_H

#include "Joystick.h"
#include "AnalogKeyboard.h"
#include "RotaryEncoder.h"
#include "Potentiometer.h"
#include "IMUReader.h"

struct Hardware {
  Joystick joyRight{36, 39, 1920, 1925, 100};
  Joystick joyLeft{13, 12, 1910, 1995, 100};
  AnalogKeyboard keyboard{34};
  RotaryEncoder encoderRight{33, 32, 35};
  RotaryEncoder encoderLeft{27, 26, 25};
  RotaryEncoder encoderMiddle{31, 30, 29};
  Potentiometer potLeft{15};
  Potentiometer potMid{4};
  Potentiometer potRight{14};
  IMUReader imu;

  Hardware() {}
};

#endif
