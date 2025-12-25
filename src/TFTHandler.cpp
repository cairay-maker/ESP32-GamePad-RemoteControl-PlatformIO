#include "TFTHandler.h"

TFTHandler::TFTHandler() : tft() {}

void TFTHandler::begin() {
  tft.init();
  tft.setRotation(3);        // Your working rotation
  clearScreen();
}

void TFTHandler::clearScreen(uint16_t color) {
  tft.fillScreen(color);
}

void TFTHandler::drawCenteredText(const char* text, int y, uint16_t color, int textSize) {
  tft.setTextColor(color);
  tft.setTextSize(textSize);
  int16_t x = (tft.width() - tft.textWidth(text)) / 2;
  tft.setCursor(x, y);
  tft.println(text);
}