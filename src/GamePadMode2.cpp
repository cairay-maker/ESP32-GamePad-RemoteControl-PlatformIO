#include "GamePadMode2.h"  // ← Include the new header
#include <Arduino.h>

GamePadMode2::GamePadMode2(TFTHandler& tftRef, Hardware& hwRef)
    : Mode(tftRef), hw(hwRef)
{
}

void GamePadMode2::enter() {
  tft.clearScreen();
  tft.drawCenteredText("Game Pad Mode 2", 8, TFT_CYAN, 1);
}

void GamePadMode2::update() {
  // Placeholder - can be empty or show something simple
  // For now, just keep the title
}

void GamePadMode2::exit() {
  // Nothing needed
}