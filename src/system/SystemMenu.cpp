#include "system/SystemMenu.h"
#include <Arduino.h>
#include <WiFi.h>

SystemMenu::SystemMenu(TFTHandler& tftRef, Hardware& hwRef)
    : Activity(tftRef, hwRef),
      axis3DView(tftRef, hwRef),
      dataView(tftRef, hwRef),
      graphicView(tftRef, hwRef),
      espNowView(tftRef, hwRef)
{}

void SystemMenu::enter() {
  currentState = MENU;
  selectedApp = 0;
  isHoldingSelect = false;
  suppressNextRelease = false;
  
  // Initialize tracker with current WiFi state
  lastWifiState = (WiFi.getMode() != WIFI_OFF);
  drawMenu(); 
}

void SystemMenu::drawMenu() {
  tft.canvas.fillSprite(TFT_BLACK);
  
  // --- WiFi Status Icon (Top Right) ---
  tft.drawWifiIcon(); 
  lastWifiState = (WiFi.getMode() != WIFI_OFF);

  tft.drawCenteredText("SYSTEM MENU", 8, TFT_CYAN, 1);

  // Draw App List
  for (int i = 0; i < numApps; i++) {
    int y = 25 + i * 15;
    if (i == selectedApp) {
      tft.canvas.setTextColor(TFT_ORANGE);
      tft.canvas.setCursor(20, y);
      tft.canvas.print("> ");
    } else {
      tft.canvas.setTextColor(TFT_WHITE);
      tft.canvas.setCursor(25, y);
    }
    tft.canvas.println(appNames[i]);
  }

  tft.drawCenteredText("UP/DOWN: Select", 105, TFT_GREENYELLOW, 1);
  tft.drawCenteredText("SELECT: Enter/Exit(Hold)", 115, TFT_GREENYELLOW, 1);
  tft.updateDisplay();
}

void SystemMenu::startApp(int index) {
  tft.canvas.fillSprite(TFT_BLACK);
  tft.updateDisplay();
  
  if (index == 0)      { currentState = ESPNOW;  espNowView.enter(); }
  else if (index == 1) { currentState = AXIS;    axis3DView.enter(); }
  else if (index == 2) { currentState = DATA;    dataView.enter(); }
  else if (index == 3) { currentState = GRAPHIC; graphicView.enter(); }
}

void SystemMenu::update() {
  // 1. Live WiFi Refresh (Detects Switch 2 change from main loop)
  bool currentWifi = (WiFi.getMode() != WIFI_OFF);
  if (currentWifi != lastWifiState && currentState == MENU) {
    drawMenu(); 
  }

  // 2. Capture hardware state ONCE to prevent noise/skipping
  String currentKey = hw.keyboard.getCurrentKey(); 
  unsigned long now = millis();

  // 3. Select Button Logic (Short Press to Enter / Long Press to Exit)
  if (currentKey == "SELECT") {
    if (!isHoldingSelect) {
      isHoldingSelect = true;
      selectHoldStartTime = now;
      suppressNextRelease = false;
    } else if (!suppressNextRelease && (now - selectHoldStartTime >= 1000)) {
      // Trigger Exit on Long Press
      if (currentState != MENU) {
        currentState = MENU;
        suppressNextRelease = true; 
        drawMenu();
        return; 
      }
    }
  } else {
    // Release detection
    if (isHoldingSelect) {
      if (!suppressNextRelease && currentState == MENU) {
        // Trigger Launch on Short Press
        if (now - selectHoldStartTime < 1000) {
          startApp(selectedApp);
        }
      }
      isHoldingSelect = false;
      suppressNextRelease = false;
    }
  }

  // 4. Menu Navigation (Only active when in MENU state)
  if (currentState == MENU) {
    String pressed = hw.keyboard.getPressedKey(); // Uses internal latch to prevent skipping
    if (pressed == "UP") {
      selectedApp = (selectedApp - 1 + numApps) % numApps;
      drawMenu(); 
    } else if (pressed == "DOWN") {
      selectedApp = (selectedApp + 1) % numApps;
      drawMenu(); 
    }
  } else {
    // 5. Run Active Sub-View Logic
    if (currentState == AXIS) axis3DView.update();
    else if (currentState == DATA) dataView.update();
    else if (currentState == GRAPHIC) graphicView.update();
    else if (currentState == ESPNOW) espNowView.update();
  }     
}

void SystemMenu::exit() {}