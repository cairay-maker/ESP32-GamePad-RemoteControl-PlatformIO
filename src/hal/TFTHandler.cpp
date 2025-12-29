#include "hal/TFTHandler.h"

// Initialize canvas with the address of tft
TFTHandler::TFTHandler() : tft(TFT_eSPI()), canvas(&tft) {}

void TFTHandler::begin() {
    tft.init();
    tft.setRotation(3); // Set to 3 as requested
    tft.fillScreen(TFT_BLACK);

    canvas.setColorDepth(16);
    // Allocate the 40KB Global Framebuffer (160x128)
    if (!canvas.createSprite(160, 128)) {
        Serial.println("RAM Error: Global Canvas failed!");
    } else {
        Serial.println("Global Canvas Ready (Rotation 3)");
    }
}

// Fixed signature to match header
void TFTHandler::clearScreen(uint16_t color) {
    canvas.fillSprite(color);
    tft.fillScreen(color);
}

void TFTHandler::drawCenteredText(const char* text, int y, uint16_t color, int size) {
    canvas.setTextSize(size);
    canvas.setTextColor(color);
    canvas.drawCentreString(text, 80, y, 1); 
}

// Added missing definition
void TFTHandler::updateDisplay() {
    canvas.pushSprite(0, 0);
}