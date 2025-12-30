#include "Hardware.h"

void Hardware::begin() {
  // Digital Pullups for Switches and Encoders
  pinMode(19, INPUT_PULLUP);
  pinMode(25, INPUT_PULLUP);
  pinMode(27, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  pinMode(4,  INPUT_PULLUP);
}

void Hardware::readAll(uint8_t modeId) {
  // 1. IMU Initialization Guard (STAYED)
  if (!imuInitialized) {
    if (imu.begin()) {
      imuInitialized = true;
      Serial.println("IMU Initialized.");
    } else {
      static unsigned long lastError = 0;
      if (millis() - lastError > 5000) {
        Serial.println("IMU Error - Retrying...");
        lastError = millis();
      }
      state.ax = state.ay = state.az = 0;
      state.gx = state.gy = state.gz = 0;
    }
  }

  // 2. ADC1 Reads (Safe always)
  joyRight.update();   delayMicroseconds(50);
  potLeft.update();    delayMicroseconds(50);
  potMid.update();     delayMicroseconds(50);
  potRight.update();   delayMicroseconds(50);

  // 3. ADC2 Conditional Logic (The WiFi Guard)
  if (WiFi.getMode() == WIFI_OFF) {
    joyLeft.update();
    state.joyLX = joyLeft.getMappedX();
    state.joyLY = joyLeft.getMappedY();
    state.joyLXRaw = joyLeft.getRawX(); 
    state.joyLYRaw = joyLeft.getRawY();
  } else {
    // Neutral values while WiFi is active to prevent crashes
    state.joyLX = 0.0f;
    state.joyLY = 0.0f;
    state.joyLXRaw = 2048;
    state.joyLYRaw = 2048;
  }

  // 4. Safe Digital/I2C Updates
  if (imuInitialized) {
    imu.update();
  }

  // 5. Data Mapping
  state.joyRX = joyRight.getMappedX();
  state.joyRY = joyRight.getMappedY();
  state.potL  = potLeft.getNormalized();
  state.potM  = potMid.getNormalized();
  state.potR  = potRight.getNormalized();
  state.joyRXRaw = joyRight.getRawX();
  state.joyRYRaw = joyRight.getRawY();
  state.potLRaw  = potLeft.getRaw();
  state.potMRaw  = potMid.getRaw();
  state.potRRaw  = potRight.getRaw();

  state.ax = imu.ax; state.ay = imu.ay; state.az = imu.az;
  state.gx = imu.gx; state.gy = imu.gy; state.gz = imu.gz;
  state.encL = encoderLeft.getCounter();
  state.encR = encoderRight.getCounter();

  // 6. Button Logic
  state.buttons = 0;
  String key = keyboard.getCurrentKey();
  if (key == "UP")     state.buttons |= (1 << 0);
  if (key == "DOWN")   state.buttons |= (1 << 1);
  if (key == "LEFT")   state.buttons |= (1 << 2);
  if (key == "RIGHT")  state.buttons |= (1 << 3);
  if (key == "SELECT") state.buttons |= (1 << 4);
  if (Switch1.isOn())  state.buttons |= (1 << 7);
  if (Switch2.isOn())  state.buttons |= (1 << 8);

  state.modeId = modeId;
}