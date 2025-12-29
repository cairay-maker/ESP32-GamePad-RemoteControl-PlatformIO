#include <Arduino.h>
#include "hal/Hardware.h"
#include "hal/TFTHandler.h"
#include "Activity.h"

// Gatekeeper Menus
#include "games/GameMenu.h"
#include "system/SystemMenu.h"

// Global objects
TFTHandler tft;
Hardware hw; 

// Menus
GameMenu gameMenu(tft, hw);           
SystemMenu systemMenu(tft, hw);

// State tracking
Activity* currentActivity = nullptr;
uint8_t currentActivityId = 0;
ControllerState lastLoggedState; 

/**
 * Log hardware values only when they change significantly.
 */

void logHardwareChanges(float rawAnalogThreshold, float imuThreshold) {
  static unsigned long lastLogTime = 0;
  if (millis() - lastLogTime < 100) return; 

  bool changed = false;

  // 1. Change Detection
  if (abs(hw.state.joyLXRaw - lastLoggedState.joyLXRaw) > rawAnalogThreshold) changed = true;
  if (abs(hw.state.joyLYRaw - lastLoggedState.joyLYRaw) > rawAnalogThreshold) changed = true;
  if (abs(hw.state.joyRXRaw - lastLoggedState.joyRXRaw) > rawAnalogThreshold) changed = true;
  if (abs(hw.state.joyRYRaw - lastLoggedState.joyRYRaw) > rawAnalogThreshold) changed = true;
  if (abs(hw.state.potLRaw  - lastLoggedState.potLRaw)  > rawAnalogThreshold) changed = true;
  if (abs(hw.state.potMRaw  - lastLoggedState.potMRaw)  > rawAnalogThreshold) changed = true;
  if (abs(hw.state.potRRaw  - lastLoggedState.potRRaw)  > rawAnalogThreshold) changed = true;
  
  // Trigger on any state change for binary/discrete inputs
  if (hw.state.buttons != lastLoggedState.buttons)         changed = true;
  if (hw.state.encL != lastLoggedState.encL)               changed = true;
  if (hw.state.encR != lastLoggedState.encR)               changed = true;
  if (abs(hw.state.ax - lastLoggedState.ax) > imuThreshold) changed = true;

  if (changed) {
    lastLogTime = millis();
    
    // Extract Switch bits (Switch1=bit 7, Switch2=bit 8)
    int sw1 = (hw.state.buttons & (1 << 7)) ? 1 : 0;
    int sw2 = (hw.state.buttons & (1 << 8)) ? 1 : 0;
    
    // Ordered: Joysticks -> Pots -> Encoders -> Keyboard -> Switches
    Serial.printf("RAW | JL:[%4d,%4d] JR:[%4d,%4d] | POT L:%4d M:%4d R:%4d | ENC L:%2d R:%2d | KEY:%-6s | SW:%d %d\n",
                  hw.state.joyLXRaw, hw.state.joyLYRaw,
                  hw.state.joyRXRaw, hw.state.joyRYRaw,
                  hw.state.potLRaw,  hw.state.potMRaw,  hw.state.potRRaw,
                  hw.state.encL,     hw.state.encR,
                  hw.keyboard.getCurrentKey().c_str(),
                  sw1, sw2);
    
    lastLoggedState = hw.state;
  }
} 

 void setup() {
  Serial.begin(115200);
  delay(2000); // Critical delay for hardware voltage stabilization

  Serial.println("\n=== ESP32 Dual-World Multi-Controller ===");
  tft.begin();
  
  // Perform initial read to set boot world
  hw.readAll(0);
  
  if (hw.Switch1.isOn()) {
    currentActivity = &gameMenu;
    currentActivityId = 1;
  } else {
    currentActivity = &systemMenu;
    currentActivityId = 2;
  }

  currentActivity->enter();
}

void loop() {
  // 1. World Selection
  bool s1 = hw.Switch1.isOn();
  Activity* nextActivity = (s1) ? (Activity*)&gameMenu : (Activity*)&systemMenu;
  uint8_t nextId = (s1) ? 1 : 2;

  // 2. Transitions
  if (nextActivity != currentActivity) {
    if (currentActivity) currentActivity->exit();
    currentActivity = nextActivity;
    currentActivityId = nextId;
    currentActivity->enter();
  }

  // 3. Global Hardware Refresh
  hw.readAll(currentActivityId);

  // 4. Logger with 100/100/0.4 Thresholds
  logHardwareChanges(200, 0.4);

  // 5. Update Activity
  if (currentActivity) {
    currentActivity->update();
  }

  delay(5);
}