#ifndef HARDWARE_H
#define HARDWARE_H

#include "hal/Joystick.h"
#include "hal/AnalogKeyboard.h"
#include "hal/RotaryEncoder.h"
#include "hal/Potentiometer.h"
#include "hal/IMUReader.h"
#include "hal/ToggleSwitch.h"
#include <Arduino.h>
#include <WiFi.h>

typedef struct __attribute__((packed)) {
  float joyLX, joyLY, joyRX, joyRY; 
  float potL, potM, potR;      
  int joyLXRaw, joyLYRaw, joyRXRaw, joyRYRaw;
  int potLRaw, potMRaw, potRRaw;
  float ax, ay, az;                 
  float gx, gy, gz;                 
  long encL, encR;            
  uint16_t buttons;                 
  uint8_t modeId;                   
} ControllerState;

struct Hardware {
  ToggleSwitch Switch1{19};
  ToggleSwitch Switch2{25};
  IMUReader imu;
  Potentiometer potLeft{35};
  Potentiometer potMid{33};
  Potentiometer potRight{32};
  RotaryEncoder encoderLeft{27, 26, -1};
  RotaryEncoder encoderRight{4, 14, -1};
  Joystick joyLeft{13, 12, 1910, 1995, 100};
  Joystick joyRight{36, 39, 1920, 1925, 100};
  AnalogKeyboard keyboard{34};

  ControllerState state;
  bool imuInitialized = false;

  Hardware() {}
  void begin(); // Setup pins and IMU
  void readAll(uint8_t modeId);
};

#endif