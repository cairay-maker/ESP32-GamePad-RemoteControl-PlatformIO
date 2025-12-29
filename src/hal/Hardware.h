#ifndef HARDWARE_H
#define HARDWARE_H

#include "hal/Joystick.h"
#include "hal/AnalogKeyboard.h"
#include "hal/RotaryEncoder.h"
#include "hal/Potentiometer.h"
#include "hal/IMUReader.h"
#include "hal/ToggleSwitch.h" // [CHANGE] Added ToggleSwitch include
#include <Arduino.h>

typedef struct __attribute__((packed)) {

  // Mapped values (for games/UI)
  float joyLX, joyLY, joyRX, joyRY; 
  float potL, potM, potR;      
  
  // RAW values (for calibration)
  int joyLXRaw, joyLYRaw, joyRXRaw, joyRYRaw;
  int potLRaw, potMRaw, potRRaw;
  
  float ax, ay, az;                 
  float gx, gy, gz;                 
  long encL, encR;            
  uint16_t buttons;                 
  uint8_t modeId;                   
} ControllerState;

struct Hardware {
  // [CHANGE] Centralized Pin Definitions (Easier to maintain)

  ToggleSwitch Switch1{19};
  ToggleSwitch Switch2{25};

  IMUReader imu;

  Potentiometer potLeft{15};
  Potentiometer potMid{4};
  Potentiometer potRight{14};

  RotaryEncoder encoderRight{33, 32, 35};
  RotaryEncoder encoderLeft{27, 26, 25};

  Joystick joyRight{36, 39, 1920, 1925, 100};
  Joystick joyLeft{13, 12, 1910, 1995, 100};

  AnalogKeyboard keyboard{34};

  ControllerState state;

  Hardware() {}

  void readAll(uint8_t modeId);
};

#endif