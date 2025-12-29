#include "Hardware.h"
#include <WiFi.h> // Include to explicitly turn it off

void Hardware::readAll(uint8_t modeId) {
  // Explicitly disable Wi-Fi to free up ADC2 if it was accidentally on
  static bool wifiOff = false;
  if (!wifiOff) {
    WiFi.mode(WIFI_OFF);
    wifiOff = true;
  }

// 1. Safety: Only update IMU if it's actually there
  static bool imuInitialized = false;
  if (!imuInitialized) {
    if (imu.begin()) {
      imuInitialized = true;
    }
  }

  // 2. Analog Reads with "Cool Down"
  // We use 100us because it's the safest margin for ESP32 ADC1/ADC2 switching
  joyLeft.update();   delayMicroseconds(10);
  joyRight.update();  delayMicroseconds(10);
  potLeft.update();   delayMicroseconds(10);
  potMid.update();    delayMicroseconds(10);
  potRight.update();  delayMicroseconds(10);

  if (imuInitialized) {
    imu.update();
  }

  // 2. Map Analog/IMU to State
  state.joyLX = joyLeft.getMappedX();
  state.joyLY = joyLeft.getMappedY();
  state.joyRX = joyRight.getMappedX();
  state.joyRY = joyRight.getMappedY();

  state.potL = potLeft.getNormalized();
  state.potM = potMid.getNormalized();
  state.potR = potRight.getNormalized();

  state.ax = imu.ax; state.ay = imu.ay; state.az = imu.az;
  state.gx = imu.gx; state.gy = imu.gy; state.gz = imu.gz;

  state.encL = encoderLeft.getCounter();
  state.encR = encoderRight.getCounter();

  // 3. Bit-Packing Buttons
  state.buttons = 0;
  String key = keyboard.getCurrentKey();
  if (key == "UP")     state.buttons |= (1 << 0);
  if (key == "DOWN")   state.buttons |= (1 << 1);
  if (key == "LEFT")   state.buttons |= (1 << 2);
  if (key == "RIGHT")  state.buttons |= (1 << 3);
  if (key == "SELECT") state.buttons |= (1 << 4);

  if (encoderLeft.isButtonPressed())  state.buttons |= (1 << 5);
  if (encoderRight.isButtonPressed()) state.buttons |= (1 << 6);

  if (Switch1.isOn()) state.buttons |= (1 << 7);
  if (Switch2.isOn()) state.buttons |= (1 << 8);

  state.modeId = modeId;
}