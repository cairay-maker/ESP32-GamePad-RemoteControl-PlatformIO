#include "games/GameMenu.h"
#include <Arduino.h>

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
  drawMenu(); 
}

void GameMenu::drawMenu() {
  tft.canvas.fillSprite(TFT_BLACK);
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
  
  if (index == 0) { currentGame = BALANCE; balanceGame.init(); }
  else if (index == 1) { currentGame = SHOOTER; spaceShooterGame.init(); }
  else if (index == 2) { currentGame = STARSHIP; starshipGame.init(); }
  else if (index == 3) { currentGame = RACING; racingGame.init(); }
  else if (index == 4) { currentGame = PONG; pongGame.init(); }
}

void GameMenu::update() {
  bool selectDown = hw.keyboard.isKeyHeld("SELECT");
  unsigned long now = millis();

  // --- Long Press / Short Press Logic ---
  if (selectDown) {
    if (!isHoldingSelect) {
      isHoldingSelect = true;
      selectHoldStartTime = now;
      suppressNextRelease = false; 
    } else if (now - selectHoldStartTime >= 1000 && !suppressNextRelease) {
      // Threshold hit: Trigger Exit
      if (currentGame != MENU) {
        currentGame = MENU;
        suppressNextRelease = true; // Block the upcoming release from triggering startGame
        drawMenu();
        return;
      }
    }
  } else {
    if (isHoldingSelect) {
      isHoldingSelect = false;
      // Only start a game if we didn't just perform a Long Press Exit
      if (!suppressNextRelease && currentGame == MENU) {
        if (now - selectHoldStartTime < 1000) {
          startGame(selectedGame);
          return;
        }
      }
      suppressNextRelease = false; 
    }
  }

  // --- Menu Navigation ---
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

  // --- Sub-Activity Update ---
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