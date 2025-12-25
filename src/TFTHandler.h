#ifndef TFTHANDLER_H
#define TFTHANDLER_H

#include <TFT_eSPI.h>

class TFTHandler {
public:
  TFTHandler();
  void begin();
  void clearScreen(uint16_t color = TFT_BLACK);
  void drawCenteredText(const char* text, int y = 60, uint16_t color = TFT_WHITE, int textSize = 2);

  // Add this: direct access to the TFT instance for advanced use
  TFT_eSPI& getTFT() { return tft; }

private:
  TFT_eSPI tft;
};

#endif