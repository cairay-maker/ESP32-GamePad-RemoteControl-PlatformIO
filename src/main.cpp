#include <Arduino.h>
#include "hal/Hardware.h"   // [CHANGE] Hardware now includes ToggleSwitch.h internally
#include "hal/TFTHandler.h"
#include "modes/GamePadMode.h"
#include "modes/IMUControlMode.h"
#include "modes/RemoteTextMode.h"
#include "modes/RemoteGraphicMode.h"

// Global objects
TFTHandler tft;
Hardware hw; // [CHANGE] hw now owns Switch1 (19) and Switch2 (25) internally

// Modes - References the same 'hw' instance for data
GamePadMode gamePadMode(tft, hw);           
IMUControlMode imuControlMode(tft, hw);     
RemoteTextMode remoteTextMode(tft, hw);
RemoteGraphicMode remoteGraphicMode(tft, hw);

// Current active mode tracking
Mode* currentMode = nullptr;
uint8_t currentModeId = 0;

void setup() {
  Serial.begin(115200);
  delay(2000); 

  Serial.println("\n=== ESP32 Dual Switch Controller ===");
  Serial.println("System: Unified HAL & ControllerState Enabled");
  Serial.println("Started - Dec 28, 2025");

  tft.begin();
  delay(500);

  // Initial mode check based on physical switch positions at boot
  bool s1 = hw.Switch1.isOn();
  bool s2 = hw.Switch2.isOn();
  
  // Logical Mode Mapping
  if (!s1 && !s2)      { currentMode = &remoteTextMode; currentModeId = 3; }
  else if (!s1 && s2) { currentMode = &remoteGraphicMode; currentModeId = 2; }
  else if (s1 && !s2) { currentMode = &imuControlMode;    currentModeId = 4; }
  else                { currentMode = &gamePadMode;       currentModeId = 1; }

  currentMode->enter();
}

void loop() {
  // 1. [CHANGE] Determine mode based on internal hardware switch states
  bool s1 = hw.Switch1.isOn();
  bool s2 = hw.Switch2.isOn();

  Mode* nextMode = nullptr;
  uint8_t nextModeId = 0;

  if (!s1 && !s2) {
    nextMode = &remoteTextMode;
    nextModeId = 3;
  } else if (!s1 && s2) {
    nextMode = &remoteGraphicMode;
    nextModeId = 2;
  } else if (s1 && !s2) {
    nextMode = &imuControlMode;
    nextModeId = 4;
  } else {
    nextMode = &gamePadMode;
    nextModeId = 1;
  }

  // 2. Handle Mode Transitions
  if (nextMode != currentMode) {
    if (currentMode) currentMode->exit();
    currentMode = nextMode;
    currentModeId = nextModeId;
    currentMode->enter();
  }

  // 3. [CHANGE] Global Sync: Refresh all sensors and bit-pack switches into state
  // This populates hw.state for use inside the currentMode->update()
  hw.readAll(currentModeId);

  // 4. Update the active mode (Modes now draw from hw.state)
  if (currentMode) {
    currentMode->update();
  }

  // Small delay for OS stability and power management
  delay(10);
}