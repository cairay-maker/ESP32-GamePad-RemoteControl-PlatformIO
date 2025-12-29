#include "modes/GamePadMode.h"
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
  // 1. Clear the canvas (RAM), not just the physical screen
  tft.canvas.fillSprite(TFT_BLACK);

  // 2. Draw Title to Canvas
  tft.drawCenteredText("Game Pad Mode", 8, TFT_CYAN, 1);

  // 3. Draw Menu Items to Canvas
  for (int i = 0; i < numGames; i++) {
    int y = 30 + i * 15;
    if (i == selectedGame) {
      tft.canvas.setTextColor(TFT_YELLOW);
      tft.canvas.setCursor(20, y);
      tft.canvas.print("> ");
    } else {
      tft.canvas.setTextColor(TFT_WHITE);
      tft.canvas.setCursor(25, y);
    }
    tft.canvas.println(gameNames[i]);
  }

  // 4. Draw Instructions to Canvas
  tft.drawCenteredText("UP/DOWN: Select", 105, TFT_ORANGE, 1);
  tft.drawCenteredText("SELECT: Play/Exit", 115, TFT_ORANGE, 1);

  // 5. [CRITICAL] Push the canvas to the physical screen
  tft.updateDisplay();
}

void GamePadMode::startGame(int index) {
  // Clear canvas before starting game
  tft.canvas.fillSprite(TFT_BLACK);
  tft.updateDisplay();
  
  balanceGame.cleanup();
  spaceShooterGame.cleanup();
  starshipGame.cleanup();
  racingGame.cleanup();
  pongGame.cleanup();

  if (index == 0) { currentGame = BALANCE; balanceGame.init(); }
  else if (index == 1) { currentGame = SHOOTER; spaceShooterGame.init(); }
  else if (index == 2) { currentGame = STARSHIP; starshipGame.init(); }
  else if (index == 3) { currentGame = RACING; racingGame.init(); }
  else if (index == 4) { currentGame = PONG; pongGame.init(); }
}

void GamePadMode::update() {
  String pressed = hw.keyboard.getPressedKey();

  // 1. Handle Selection/Exit
  if (pressed == "SELECT") {
    if (currentGame != MENU) {
      // Cleanup happens inside drawMenu or before switching
      currentGame = MENU;
      drawMenu();
      return; 
    } else {
      startGame(selectedGame);
      return;
    }
  }

  // 2. Handle Menu Navigation
  if (currentGame == MENU) {
    if (pressed == "UP") {
      selectedGame = (selectedGame - 1 + numGames) % numGames;
      drawMenu(); // This draws to canvas and NOW calls updateDisplay internally
    } else if (pressed == "DOWN") {
      selectedGame = (selectedGame + 1) % numGames;
      drawMenu(); // This draws to canvas and NOW calls updateDisplay internally
    }
    return; 
  }

  // 3. Game Run Loops (Only reached if NOT in MENU)
  // Erase the canvas before the game draws its frame
  tft.canvas.fillSprite(TFT_BLACK);

  switch (currentGame) {
    case BALANCE:
      balanceGame.update();
      balanceGame.draw();
      break;
    case SHOOTER:
      spaceShooterGame.update();
      spaceShooterGame.draw();
      break;
    case STARSHIP:
      starshipGame.update();
      starshipGame.draw();
      break;
    case RACING:
      racingGame.update();
      racingGame.draw();
      break;
    case PONG:
      pongGame.update();
      pongGame.draw();
      break;
    default:
      break;
  }

  // Final push for the active game frame
  tft.updateDisplay();
}

void GamePadMode::exit() {
  balanceGame.cleanup();
  spaceShooterGame.cleanup();
  starshipGame.cleanup();
  racingGame.cleanup();
  pongGame.cleanup();
}