#include "IMUControlMode.h"
#include <Arduino.h>

static const int FRAME_W = 140;
static const int FRAME_H = 80; 
static const int FRAME_X = 10; 
static const int FRAME_Y = 25;                 

IMUControlMode::IMUControlMode(TFTHandler& tftRef, Hardware& hwRef)
    : Mode(tftRef), hw(hwRef), sprite(&tftRef.getTFT()) {}

void IMUControlMode::enter() {
    tft.clearScreen();
    tft.drawCenteredText("IMU 3D ISOMETRIC AXIS", 8, TFT_CYAN, 1);
    tft.drawCenteredText("Press Select to Calibrate", 115, TFT_LIGHTGREY, 1);
    tft.getTFT().drawRect(FRAME_X - 1, FRAME_Y - 1, FRAME_W + 2, FRAME_H + 2, 0x7BEF);

    if (!sprite.created()) sprite.createSprite(FRAME_W, FRAME_H);
}

void IMUControlMode::update() {
    if (hw.keyboard.getPressedKey() == "SELECT") hw.imu.calibrate();

    hw.imu.update();
    
    float p = hw.imu.getPitch() * (PI / 180.0f);
    float r = hw.imu.getRoll() * (PI / 180.0f);

    sprite.fillSprite(TFT_BLACK);

    int scx = FRAME_W / 2;
    int scy = (FRAME_H / 2) + 10; 

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
        sprite.drawLine(p1.first, p1.second, p2.first, p2.second, color);
        sprite.drawLine(p1.first + 1, p1.second, p2.first + 1, p2.second, color);
    };

    auto origin = project(0, 0, 0);

    // --- 1. DRAW MOVING ELLIPSE ---
    // We treat the ellipse as a ring on the X-Y plane (Z=0)
    // Width 80 (radius 40/baseScale = 1.33)
    // Height 40 (radius 20/baseScale = 0.66)
    float rx = 1.33f; 
    float ry = 0.66f;
    int segments = 16;
    std::pair<int, int> prevPt;

    for (int i = 0; i <= segments; i++) {
        float angle = i * (2.0f * PI / segments);
        auto currPt = project(cos(angle) * rx, sin(angle) * ry, 0);
        
        if (i > 0) {
            sprite.drawLine(prevPt.first, prevPt.second, currPt.first, currPt.second, 0x7BEF); // Grey outline
        }
        prevPt = currPt;
    }

    // --- 2. DRAW AXES ---
    auto xAxis = project(1.0f, 0, 0);
    drawThickLine(origin, xAxis, TFT_RED);
    
    auto yAxis = project(0, 0.7f, 0); 
    drawThickLine(origin, yAxis, TFT_GREEN);
    
    auto zAxis = project(0, 0, 1.0f);
    drawThickLine(origin, zAxis, TFT_BLUE);

    // --- 3. TEXT ---
    sprite.setTextColor(TFT_WHITE);
    sprite.setCursor(2, 2);
    sprite.printf("P:%+05.1f R:%+05.1f Y:%+05.1f", hw.imu.getPitch(), hw.imu.getRoll(), hw.imu.getYawRate());

    sprite.pushSprite(FRAME_X, FRAME_Y);
}

void IMUControlMode::exit() {
    if (sprite.created()) sprite.deleteSprite();
}