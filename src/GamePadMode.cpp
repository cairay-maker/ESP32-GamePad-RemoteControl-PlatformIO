#include "GamePadMode.h"

void GamePadMode::enter() {
  tft.clearScreen();
  tft.drawCenteredText("Game Pad", 60, TFT_GREEN, 3);
}

void GamePadMode::update() {
  // Future: joystick, buttons, etc.
}

void GamePadMode::exit() {
  // Cleanup if needed
}