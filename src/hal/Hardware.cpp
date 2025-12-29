#include "Hardware.h"
#include <WiFi.h>

void Hardware::readAll(uint8_t modeId) {
  // 1. Disable Wi-Fi once to stabilize ADC
  static bool wifiOff = false;
  if (!wifiOff) {
    WiFi.mode(WIFI_OFF);
    wifiOff = true;
  }

  // 2. IMU Initialization Guard (Safety check added back)
  static bool imuInitialized = false;
  if (!imuInitialized) {
    if (imu.begin()) {
      imuInitialized = true;
      Serial.println("IMU Initialized Successfully.");
    } else {
      // Don't log this every frame or it will crash the serial buffer
      static unsigned long lastError = 0;
      if (millis() - lastError > 5000) {
        Serial.println("IMU Not Found - Check Wiring.");
        lastError = millis();
      }
    }
  }

  // 3. Analog Reads with Settling Delays
  // Increased to 50us to prevent ADC Lock errors during Serial spam
  joyLeft.update();   delayMicroseconds(50);
  joyRight.update();  delayMicroseconds(50);
  potLeft.update();   delayMicroseconds(50);
  potMid.update();    delayMicroseconds(50);
  potRight.update();  delayMicroseconds(50);

  // 4. Conditional IMU Update
  if (imuInitialized) {
    imu.update();
  }

  // 5. Mapping
  state.joyLX = joyLeft.getMappedX();
  state.joyLY = joyLeft.getMappedY();
  state.joyRX = joyRight.getMappedX();
  state.joyRY = joyRight.getMappedY();

  state.potL = potLeft.getNormalized();
  state.potM = potMid.getNormalized();
  state.potR = potRight.getNormalized();

  // Capture RAW Data (Directly from the sensors)
  state.joyLXRaw = joyLeft.getRawX(); 
  state.joyLYRaw = joyLeft.getRawY();
  state.joyRXRaw = joyRight.getRawX();
  state.joyRYRaw = joyRight.getRawY();
  
  state.potLRaw  = potLeft.getRaw();
  state.potMRaw  = potMid.getRaw();
  state.potRRaw  = potRight.getRaw();

  state.ax = imu.ax; state.ay = imu.ay; state.az = imu.az;
  state.gx = imu.gx; state.gy = imu.gy; state.gz = imu.gz;

  state.encL = encoderLeft.getCounter();
  state.encR = encoderRight.getCounter();

  // 6. Bit-Packing
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