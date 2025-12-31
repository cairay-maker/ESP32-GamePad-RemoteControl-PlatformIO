#include "games/GameMenu.h"
#include <Arduino.h>
#include <WiFi.h>

GameMenu::GameMenu(TFTHandler& tftRef, Hardware& hwRef)
    : Activity(tftRef, hwRef),
      pongGame(tftRef, hwRef),
      racingGame(tftRef, hwRef),
      spaceShooterGame(tftRef, hwRef),
      starshipGame(tftRef, hwRef),
      balanceGame(tftRef, hwRef)
{}

void GameMenu::enter() {
  currentGame = MENU;
  selectedGame = 0;
  isHoldingSelect = false;
  suppressNextRelease = false;
  
  // Initialize tracker
  lastWifiState = (WiFi.getMode() != WIFI_OFF);
  drawMenu(); 
}

void GameMenu::drawMenu() {
  tft.canvas.fillSprite(TFT_BLACK);
  
  // --- WiFi Status Icon (Top Right) ---
  tft.drawWifiIcon(); 
  lastWifiState = (WiFi.getMode() != WIFI_OFF);

  tft.drawCenteredText("GAME MENU", 8, TFT_CYAN, 1);

  for (int i = 0; i < numGames; i++) {
    int y = 25 + i * 15;
    if (i == selectedGame) {
      tft.canvas.setTextColor(TFT_ORANGE);
      tft.canvas.setCursor(20, y);
      tft.canvas.print("> ");
    } else {
      tft.canvas.setTextColor(TFT_WHITE);
      tft.canvas.setCursor(25, y);
    }
    tft.canvas.println(gameNames[i]);
  }

  tft.drawCenteredText("UP/DOWN: Select", 105, TFT_GREENYELLOW, 1);
  tft.drawCenteredText("SELECT: Enter/Exit(Hold)", 115, TFT_GREENYELLOW, 1);
  tft.updateDisplay();
}

void GameMenu::startGame(int index) {
  tft.canvas.fillSprite(TFT_BLACK);
  tft.updateDisplay();
  
  if (index == 0)      { currentGame = BALANCE;  balanceGame.init(); }
  else if (index == 1) { currentGame = SHOOTER;  spaceShooterGame.init(); }
  else if (index == 2) { currentGame = STARSHIP; starshipGame.init(); }
  else if (index == 3) { currentGame = RACING;   racingGame.init(); }
  else if (index == 4) { currentGame = PONG;     pongGame.init(); }
}

void GameMenu::update() {
  // 1. Live WiFi Refresh (Reacts to Switch 2 change)
  bool currentWifi = (WiFi.getMode() != WIFI_OFF);
  if (currentWifi != lastWifiState && currentGame == MENU) {
    drawMenu();
  }

  // 2. Capture hardware state ONCE
  String currentKey = hw.keyboard.getCurrentKey(); 
  unsigned long now = millis();

  // 3. SELECT Logic (Short/Long Press)
  if (currentKey == "SELECT") {
    if (!isHoldingSelect) {
      isHoldingSelect = true;
      selectHoldStartTime = now;
      suppressNextRelease = false; 
    } else if (now - selectHoldStartTime >= 1000 && !suppressNextRelease) {
      if (currentGame != MENU) {
        currentGame = MENU;
        suppressNextRelease = true; 
        drawMenu();
        return;
      }
    }
  } else {
    if (isHoldingSelect) {
      if (!suppressNextRelease && currentGame == MENU) {
        if (now - selectHoldStartTime < 1000) {
          startGame(selectedGame);
          return;
        }
      }
      isHoldingSelect = false;
      suppressNextRelease = false; 
    }
  }

  // 4. Menu Navigation
  if (currentGame == MENU) {
    String pressed = hw.keyboard.getPressedKey();
    if (pressed == "UP") {
      selectedGame = (selectedGame - 1 + numGames) % numGames;
      drawMenu(); 
    } else if (pressed == "DOWN") {
      selectedGame = (selectedGame + 1) % numGames;
      drawMenu(); 
    }
    return; 
  }

  // 5. Sub-Game Update
  tft.canvas.fillSprite(TFT_BLACK);
  switch (currentGame) {
    case BALANCE:  balanceGame.update();      balanceGame.draw(); break;
    case SHOOTER:  spaceShooterGame.update(); spaceShooterGame.draw(); break;
    case STARSHIP: starshipGame.update();     starshipGame.draw(); break;
    case RACING:   racingGame.update();       racingGame.draw(); break;
    case PONG:     pongGame.update();         pongGame.draw(); break;
    default: break;
  }
  tft.updateDisplay();
}

void GameMenu::exit() {}