#include "GamePadMode.h"
#include <Arduino.h>

GamePadMode::GamePadMode(TFTHandler& tftRef, Hardware& hwRef)
    : Mode(tftRef),
      hw(hwRef),
      pongGame(tftRef, hwRef),
      racingGame(tftRef, hwRef),
      spaceShooterGame(tftRef, hwRef)
{
}

void GamePadMode::enter() {
  currentGame = MENU;
  selectedGame = 0;
  drawMenu(); // Draw once upon entry
}

void GamePadMode::drawMenu() {
  tft.clearScreen();
  tft.drawCenteredText("Game Pad Mode", 8, TFT_CYAN, 1);

  for (int i = 0; i < numGames; i++) {
    int y = 40 + i * 20;
    if (i == selectedGame) {
      tft.getTFT().setTextColor(TFT_YELLOW);
      tft.getTFT().setCursor(25, y);
      tft.getTFT().print("> ");
    } else {
      tft.getTFT().setTextColor(TFT_WHITE);
      tft.getTFT().setCursor(35, y);
    }
    tft.getTFT().println(gameNames[i]);
  }

  tft.drawCenteredText("UP/DOWN: Select", 105, TFT_ORANGE, 1);
  tft.drawCenteredText("SELECT: Play/Exit", 115, TFT_ORANGE, 1);
}

void GamePadMode::startGame(int index) {
  // Clear RAM of any "ghost" sprites before starting
  pongGame.cleanup();
  racingGame.cleanup();
  spaceShooterGame.cleanup();

  if (index == 0) {
    currentGame = PONG;
    pongGame.init();
  } else if (index == 1) {
    currentGame = FLAPPY; // This refers to RacingGame in your enum
    racingGame.init();
  } else if (index == 2) {
    currentGame = SHOOTER;
    spaceShooterGame.init();
  }
}

void GamePadMode::update() {
  // 1. Always check the physical hardware state
  String pressed = hw.keyboard.getPressedKey();

  // 2. Universal Back/Exit Logic
  if (pressed == "SELECT") {
    if (currentGame != MENU) {
      // Cleanup the active game's RAM
      if (currentGame == PONG) pongGame.cleanup();
      else if (currentGame == FLAPPY) racingGame.cleanup();
      else if (currentGame == SHOOTER) spaceShooterGame.cleanup();
      
      currentGame = MENU;
      drawMenu();
      return; 
    } else {
      // If already in MENU, start the selected game
      startGame(selectedGame);
      return;
    }
  }

  // 3. Menu Navigation
  if (currentGame == MENU) {
    if (pressed == "UP") {
      selectedGame = (selectedGame - 1 + numGames) % numGames;
      drawMenu();
    } else if (pressed == "DOWN") {
      selectedGame = (selectedGame + 1) % numGames;
      drawMenu();
    }
    return; // Don't run game updates if in menu
  }

  // 4. Game Run Loops
  // These only run if currentGame != MENU
  if (currentGame == PONG) {
    pongGame.update();
    pongGame.draw();
    if (!pongGame.isRunning()) { currentGame = MENU; drawMenu(); }
  } 
  else if (currentGame == FLAPPY) {
    racingGame.update();
    racingGame.draw();
    if (!racingGame.isRunning()) { currentGame = MENU; drawMenu(); }
  } 
  else if (currentGame == SHOOTER) {
    spaceShooterGame.update();
    spaceShooterGame.draw();
    if (!spaceShooterGame.isRunning()) { currentGame = MENU; drawMenu(); }
  }
}

void GamePadMode::exit() {
  // Ensure all RAM is freed when leaving GamePadMode entirely
  pongGame.cleanup();
  racingGame.cleanup();
  spaceShooterGame.cleanup();
}