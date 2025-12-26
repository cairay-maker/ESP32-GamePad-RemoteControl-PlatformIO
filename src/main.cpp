#include <Arduino.h>
#include "TFTHandler.h"
#include "ToggleSwitch.h"
#include "GamePadMode.h"
#include "RemoteControlMode.h"
#include "RemoteGraphicMode.h"

#define SWITCH_1_PIN 19  // Primary switch (e.g., master power / mode selector)
#define SWITCH_2_PIN 25  // Secondary switch (for future sub-modes or options)

// Global objects
TFTHandler tft;
ToggleSwitch Switch1(SWITCH_1_PIN);  // Switch1 - Main control
ToggleSwitch Switch2(SWITCH_2_PIN);  // Switch2 - Secondary control

// Shared hardware objects (joysticks, pots, encoders, IMU)
Hardware hw;

// === Global switch states – accessible from any mode ===
bool Switch1On = false;   // Current state of Switch1
bool Switch2On = false;   // Current state of Switch2

// Modes
GamePadMode gamePadMode(tft);
RemoteControlMode remoteControlMode(tft, hw);
RemoteGraphicMode remoteGraphicMode(tft, hw);

// Current active mode
Mode* currentMode = nullptr;

void setup() {
  Serial.begin(115200);
  delay(2000);  // Critical boot delay for stability

  Serial.println("\n=== ESP32 Dual Switch Controller ===");
  Serial.println("Switch1 (Pin 19): Primary Mode");
  Serial.println("Switch2 (Pin 25): Secondary / Future Options");
  Serial.println("Current: Switch1 OFF → Remote Control Mode");
  Serial.println("         Switch1 ON  → Game Pad Mode");
  Serial.println("Started - Dec 25, 2025");

  tft.begin();
  delay(500);

  // Default to Remote Control mode
  currentMode = &remoteControlMode;
  currentMode->enter();
}

void loop() {
  // === Update global switch states every loop ===
  Switch1On = Switch1.isOn();
  Switch2On = Switch2.isOn();

  // === Mode switching ===
  if (Switch1On && currentMode != &gamePadMode) {
    if (currentMode) currentMode->exit();
    currentMode = &gamePadMode;
    currentMode->enter();
  }
  else if (!Switch1On) {
    // When Switch1 is OFF we can choose between RemoteControl text mode
    // and the new graphical joystick mode using Switch2.
    if (Switch2On && currentMode != &remoteGraphicMode) {
      if (currentMode) currentMode->exit();
      currentMode = &remoteGraphicMode;
      currentMode->enter();
    } else if (!Switch2On && currentMode != &remoteControlMode) {
      if (currentMode) currentMode->exit();
      currentMode = &remoteControlMode;
      currentMode->enter();
    }
  }

  // Run current mode update
  if (currentMode) {
    currentMode->update();
  }

  delay(10);
}