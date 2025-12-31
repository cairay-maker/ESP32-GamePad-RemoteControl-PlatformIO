#include <Arduino.h>
#include "hal/Hardware.h"
#include "hal/TFTHandler.h"
#include "hal/ESPNowHandler.h"
#include "Activity.h"

// Gatekeeper Menus
#include "games/GameMenu.h"
#include "system/SystemMenu.h"

// Global objects
TFTHandler tft;
Hardware hw; 
ESPNowHandler espNow;

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

  // Change Detection
  if (abs(hw.state.joyLXRaw - lastLoggedState.joyLXRaw) > rawAnalogThreshold) changed = true;
  if (abs(hw.state.joyLYRaw - lastLoggedState.joyLYRaw) > rawAnalogThreshold) changed = true;
  if (abs(hw.state.joyRXRaw - lastLoggedState.joyRXRaw) > rawAnalogThreshold) changed = true;
  if (abs(hw.state.joyRYRaw - lastLoggedState.joyRYRaw) > rawAnalogThreshold) changed = true;
  if (abs(hw.state.potLRaw  - lastLoggedState.potLRaw)  > rawAnalogThreshold) changed = true;
  if (abs(hw.state.potMRaw  - lastLoggedState.potMRaw)  > rawAnalogThreshold) changed = true;
  if (abs(hw.state.potRRaw  - lastLoggedState.potRRaw)  > rawAnalogThreshold) changed = true;
  
  if (hw.state.buttons != lastLoggedState.buttons)         changed = true;
  if (hw.state.encL != lastLoggedState.encL)               changed = true;
  if (hw.state.encR != lastLoggedState.encR)               changed = true;

  if (changed) {
    lastLogTime = millis();
    int sw1 = (hw.state.buttons & (1 << 7)) ? 1 : 0;
    int sw2 = (hw.state.buttons & (1 << 8)) ? 1 : 0;
    const char* wifiStatus = (WiFi.getMode() == WIFI_OFF) ? "OFF" : "ON ";
    
    Serial.printf("WIFI:%s | JL:[%4d,%4d] JR:[%4d,%4d] | POT L:%4d M:%4d R:%4d | SW:%d %d\n",
                  wifiStatus,
                  hw.state.joyLXRaw, hw.state.joyLYRaw,
                  hw.state.joyRXRaw, hw.state.joyRYRaw,
                  hw.state.potLRaw,  hw.state.potMRaw,  hw.state.potRRaw,
                  sw1, sw2);
    
    lastLoggedState = hw.state;
  }
} 

void setup() {
  Serial.begin(115200);
  delay(1000); 

  hw.begin(); // Initializes pinModes
  tft.begin();
  
  // Sync WiFi with physical Switch 2 immediately
  if (hw.Switch2.isOn()) {
    WiFi.mode(WIFI_STA);
    espNow.begin();
    Serial.println("INIT: WiFi ON (Match Switch 2)");
  } else {
    WiFi.mode(WIFI_OFF);
    Serial.println("INIT: WiFi OFF (Match Switch 2)");
  }

  // Perform initial read
  hw.readAll(0);

  // Initial Activity selection based on Switch 1
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
// --- 1. WiFi Hardware Master (Switch 2) ---
static bool lastS2 = (WiFi.getMode() != WIFI_OFF); // Initialize to actual state
bool s2 = hw.Switch2.isOn(); // GPIO 25

if (s2 != lastS2) { 
  if (s2) {
    WiFi.mode(WIFI_STA);
    espNow.begin();
    Serial.println(">>> WIFI RADIO: ON");
  } else {
    WiFi.mode(WIFI_OFF);
    Serial.println(">>> WIFI RADIO: OFF (ADC2 Unlocked)");
  }
  lastS2 = s2;
}

  // --- 2. Activity Selection (Switch 1) ---
  bool s1 = hw.Switch1.isOn();
  Activity* nextActivity = (s1) ? (Activity*)&gameMenu : (Activity*)&systemMenu;
  if (nextActivity != currentActivity) {
    if (currentActivity) currentActivity->exit();
    currentActivity = nextActivity;
    currentActivity->enter();
    Serial.printf("ACTIVITY: Switched to %s\n", s1 ? "Games" : "System");
  }

  // --- 3. Hardware & Activity Update ---
  hw.readAll(currentActivityId);

  if (currentActivity) {
    currentActivity->update();
  }

  // --- 4. ESP-NOW TRANSMIT ---
  // Only send if Switch 2 is ON. This happens every loop for low latency.
  if (hw.Switch2.isOn()) {
    espNow.send(hw); 
  }

  if (currentActivity) {
    currentActivity->update();
  }
  
  // Log values occasionally (not every loop!) to prevent lag
  logHardwareChanges(200, 0.4);

  delay(5); 
}