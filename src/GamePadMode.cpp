#include "GamePadMode.h"
#include <Arduino.h>

GamePadMode::GamePadMode(TFTHandler& tftRef, Hardware& hwRef)
    : Mode(tftRef),
      hw(hwRef),
      pongGame(tftRef, hwRef),
      racingGame(tftRef, hwRef),
      spaceShooterGame(tftRef, hwRef),
      starshipGame(tftRef, hwRef),
      balanceGame(tftRef, hwRef)
{
}

void GamePadMode::enter() {
  currentGame = MENU;
  selectedGame = 0;
  drawMenu(); 
}

void GamePadMode::drawMenu() {
  tft.clearScreen();
  tft.drawCenteredText("Game Pad Mode", 8, TFT_CYAN, 1);

  for (int i = 0; i < numGames; i++) {
    int y = 25 + i * 15;
    if (i == selectedGame) {
      tft.getTFT().setTextColor(TFT_YELLOW);
      tft.getTFT().setCursor(20, y);
      tft.getTFT().print("> ");
    } else {
      tft.getTFT().setTextColor(TFT_WHITE);
      tft.getTFT().setCursor(25, y);
    }
    tft.getTFT().println(gameNames[i]);
  }

  tft.drawCenteredText("UP/DOWN: Select", 105, TFT_ORANGE, 1);
  tft.drawCenteredText("SELECT: Play/Exit", 115, TFT_ORANGE, 1);
}

void GamePadMode::startGame(int index) {
  // 1. Clear the WHOLE screen to remove menu text and orange instructions
  tft.clearScreen();
  
  // 2. Clear RAM of any "ghost" sprites before starting
  balanceGame.cleanup();
  spaceShooterGame.cleanup();
  starshipGame.cleanup();
  racingGame.cleanup();
  pongGame.cleanup();

  // 3. Mapping based on your requested order: 
  // 0:BALANCE, 1:SHOOTER, 2:STARSHIP, 3:RACING, 4:PONG
  if (index == 0) {
    currentGame = BALANCE;
    balanceGame.init();
  } else if (index == 1) {
    currentGame = SHOOTER;
    spaceShooterGame.init();
  } else if (index == 2) {
    currentGame = STARSHIP;
    starshipGame.init();
  } else if (index == 3) {
    currentGame = RACING;
    racingGame.init();
  } else if (index == 4) {
    currentGame = PONG;
    pongGame.init();
  }
}

void GamePadMode::update() {
  // 1. Check physical hardware state
  String pressed = hw.keyboard.getPressedKey();

  // 2. Universal Back/Exit Logic (SELECT button)
  if (pressed == "SELECT") {
    if (currentGame != MENU) {
      // Cleanup the active game's RAM based on current state
      if (currentGame == BALANCE) balanceGame.cleanup();
      else if (currentGame == SHOOTER) spaceShooterGame.cleanup();
      else if (currentGame == STARSHIP) starshipGame.cleanup();
      else if (currentGame == RACING) racingGame.cleanup();
      else if (currentGame == PONG) pongGame.cleanup();

      currentGame = MENU;
      drawMenu();
      return; 
    } else {
      // If in MENU, start the highlighted game
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
    return; 
  }

  // 4. Game Run Loops (Mapped to GameState enum)
  if (currentGame == BALANCE) {
    balanceGame.update();
    balanceGame.draw();
    if (!balanceGame.isRunning()) { currentGame = MENU; drawMenu(); }
  } 
  else if (currentGame == SHOOTER) {
    spaceShooterGame.update();
    spaceShooterGame.draw();
    if (!spaceShooterGame.isRunning()) { currentGame = MENU; drawMenu(); }
  }
  else if (currentGame == STARSHIP) {
    starshipGame.update();
    starshipGame.draw();
    if (!starshipGame.isRunning()) { currentGame = MENU; drawMenu(); }
  }
  else if (currentGame == RACING) {
    racingGame.update();
    racingGame.draw();
    if (!racingGame.isRunning()) { currentGame = MENU; drawMenu(); }
  } 
  else if (currentGame == PONG) {
    pongGame.update();
    pongGame.draw();
    if (!pongGame.isRunning()) { currentGame = MENU; drawMenu(); }
  }
}

void GamePadMode::exit() {
  // Ensure all RAM is freed when leaving GamePadMode entirely
  balanceGame.cleanup();
  spaceShooterGame.cleanup();
  starshipGame.cleanup();
  racingGame.cleanup();
  pongGame.cleanup();
}