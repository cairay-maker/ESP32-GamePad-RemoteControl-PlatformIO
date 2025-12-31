#ifndef TFT_HANDLER_H
#define TFT_HANDLER_H

#include <TFT_eSPI.h>

class TFTHandler {
public:
    TFTHandler();
    void begin();
    void clearScreen(uint16_t color = 0x0000); // Matches the candidate note
    void drawCenteredText(const char* text, int y, uint16_t color, int size);
    void updateDisplay(); // Added this declaration
    void drawWifiIcon();

    TFT_eSPI& getTFT() { return tft; }
    TFT_eSprite canvas; 

private:
    TFT_eSPI tft;
};

#endif