#include "system/SystemMenu.h"
#include <Arduino.h>

SystemMenu::SystemMenu(TFTHandler& tftRef, Hardware& hwRef)
    : Activity(tftRef, hwRef),
      axis3DView(tftRef, hwRef),
      dataView(tftRef, hwRef),
      graphicView(tftRef, hwRef)
{}

void SystemMenu::enter() {
  currentState = MENU;
  selectedApp = 0;
  isHoldingSelect = false;
  suppressNextRelease = false;
  drawMenu(); 
}

void SystemMenu::drawMenu() {
  tft.canvas.fillSprite(TFT_BLACK);
  tft.drawCenteredText("SYSTEM MENU", 8, TFT_CYAN, 1);

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
  if (index == 0)      { currentState = AXIS;    axis3DView.enter(); }
  else if (index == 1) { currentState = DATA;    dataView.enter(); }
  else if (index == 2) { currentState = GRAPHIC; graphicView.enter(); }
}

void SystemMenu::update() {
  bool selectDown = hw.keyboard.isKeyHeld("SELECT");
  unsigned long now = millis();

  // --- Logic Timer ---
  if (selectDown) {
    if (!isHoldingSelect) {
      isHoldingSelect = true;
      selectHoldStartTime = now;
      suppressNextRelease = false;
    } else if (now - selectHoldStartTime >= 1000 && !suppressNextRelease) {
      if (currentState != MENU) {
        currentState = MENU;
        suppressNextRelease = true; 
        drawMenu();
        return;
      }
    }
  } else {
    if (isHoldingSelect) {
      isHoldingSelect = false;
      if (!suppressNextRelease && currentState == MENU) {
        if (now - selectHoldStartTime < 1000) {
          startApp(selectedApp);
          return;
        }
      }
      suppressNextRelease = false;
    }
  }

  // --- Menu Nav ---
  if (currentState == MENU) {
    String pressed = hw.keyboard.getPressedKey();
    if (pressed == "UP") {
      selectedApp = (selectedApp - 1 + numApps) % numApps;
      drawMenu(); 
    } else if (pressed == "DOWN") {
      selectedApp = (selectedApp + 1) % numApps;
      drawMenu(); 
    }
    return; 
  }

  // --- Sub-App Logic ---
  if (currentState == AXIS) axis3DView.update();
  else if (currentState == DATA) dataView.update();
  else if (currentState == GRAPHIC) graphicView.update();
}

void SystemMenu::exit() {}