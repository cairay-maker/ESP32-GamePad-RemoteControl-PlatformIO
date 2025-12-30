#include "system/Axis3DView.h"
#include <Arduino.h>

// Dimensions for the 3D "Viewport"
static const int FRAME_W = 140;
static const int FRAME_H = 80; 
static const int FRAME_X = 10; 
static const int FRAME_Y = 25;                 

Axis3DView::Axis3DView(TFTHandler& tftRef, Hardware& hwRef)
    : Activity(tftRef, hwRef) { // FIXED: Pass both to Activity base class
}

void Axis3DView::enter() {
    tft.canvas.fillSprite(TFT_BLACK);
}

void Axis3DView::update() {
    TFT_eSprite& c = tft.canvas; // Inherited from Activity
    c.fillSprite(TFT_BLACK);

    tft.drawCenteredText("AXIS 3D VIEW - IMU", 8, TFT_CYAN, 1);
    tft.drawCenteredText("Press Select to Calibrate", 115, TFT_LIGHTGREY, 1);
    
    c.drawRect(FRAME_X - 1, FRAME_Y - 1, FRAME_W + 2, FRAME_H + 2, 0x7BEF);

    // Use 'hw' inherited from Activity
    if (hw.keyboard.getPressedKey() == "SELECT") {
        hw.imu.calibrate();
    }

    float p = hw.imu.getPitch() * (PI / 180.0f);
    float r = hw.imu.getRoll() * (PI / 180.0f);

    int scx = FRAME_X + (FRAME_W / 2);
    int scy = FRAME_Y + (FRAME_H / 2) + 10; 

    // --- Projection Engine ---
    auto project = [&](float x, float y, float z) {
        float y1 = y * cos(p) - z * sin(p);
        float z1 = y * sin(p) + z * cos(p);
        float x2 = x * cos(r) + z1 * sin(r);
        float z2 = -x * sin(r) + z1 * cos(r);

        float isoX = x2 - (y1 * 0.707f); 
        float isoY = -z2 + (y1 * 0.707f); 

        float baseScale = 30.0f; 
        return std::pair<int, int>((int)(isoX * baseScale) + scx, (int)(isoY * baseScale) + scy);
    };

    auto drawThickLine = [&](std::pair<int, int> p1, std::pair<int, int> p2, uint16_t color) {
        c.drawLine(p1.first, p1.second, p2.first, p2.second, color);
        c.drawLine(p1.first + 1, p1.second, p2.first + 1, p2.second, color);
    };

    auto origin = project(0, 0, 0);

    // --- 3D Drawing ---
    float rx = 1.33f; 
    float ry = 0.66f;
    int segments = 16;
    std::pair<int, int> prevPt;

    for (int i = 0; i <= segments; i++) {
        float angle = i * (2.0f * PI / segments);
        auto currPt = project(cos(angle) * rx, sin(angle) * ry, 0);
        if (i > 0) c.drawLine(prevPt.first, prevPt.second, currPt.first, currPt.second, 0x7BEF);
        prevPt = currPt;
    }

    drawThickLine(origin, project(1.0f, 0, 0), TFT_RED);   // X
    drawThickLine(origin, project(0, 0.7f, 0), TFT_GREEN); // Y
    drawThickLine(origin, project(0, 0, 1.0f), TFT_BLUE);  // Z

    char buf[64];
    snprintf(buf, sizeof(buf), "P:%+05.1f R:%+05.1f Y:%+05.1f", 
             hw.imu.getPitch(), hw.imu.getRoll(), hw.imu.getYawRate());
    
    c.setTextColor(TFT_WHITE);
    c.setCursor(FRAME_X + 2, FRAME_Y + 2);
    c.print(buf);

    tft.updateDisplay();
}

void Axis3DView::exit() {
}