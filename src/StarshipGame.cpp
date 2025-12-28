#include "StarshipGame.h"
#include <Arduino.h>

static const int FRAME_W = 140;
static const int FRAME_H = 80;
static const int FRAME_X = 10;
static const int FRAME_Y = 25;

StarshipGame::StarshipGame(TFTHandler& tftRef, Hardware& hwRef)
    : Mode(tftRef), hw(hwRef), sprite(&tftRef.getTFT()) {}

void StarshipGame::init() {
    if (!sprite.created()) {
        sprite.createSprite(FRAME_W, FRAME_H);
    }
    
    // --- Automatic Calibration ---
    // Capture the current IMU position as "Level" the moment the game starts
    hw.imu.update(); 
    pitchOffset = hw.imu.getPitch();
    rollOffset = hw.imu.getRoll();
    
    state = PLAYING; // Start playing immediately
    score = 0;
    resetObstacle();
}

void StarshipGame::cleanup() {
    if (sprite.created()) {
        sprite.deleteSprite();
    }
}

void StarshipGame::enter() {
    tft.clearScreen();
    // Brief splash text so the user knows calibration is happening
    tft.drawCenteredText("CALIBRATING...", 50, TFT_CYAN, 1);
    delay(200); // Small delay to let the sensor settle
    init();
}

void StarshipGame::exit() {
    cleanup();
}

void StarshipGame::resetObstacle() {
    obstacleZ = 100.0f;
    obsX = (float)random(-30, 31);
    obsY = (float)random(-20, 21);
}

std::pair<int, int> StarshipGame::project(float x, float y, float z) {
    float factor = 50.0f / (z + 0.001f); 
    return {(int)(x * factor) + (FRAME_W / 2), (int)(y * factor) + (FRAME_H / 2)};
}

void StarshipGame::update() {
    hw.imu.update();
    sprite.fillSprite(TFT_BLACK);

    if (state == PLAYING) {
        // Use the offsets captured during init()
        float p = (hw.imu.getPitch() - pitchOffset) * (PI / 180.0f);
        float r = (hw.imu.getRoll() - rollOffset) * (PI / 180.0f);

        // 1. Draw Tunnel
        uint16_t tunnelColor = 0x3186;
        auto tl = project(-60, -40, 5); auto tr = project(60, -40, 5);
        auto bl = project(-60, 40, 5);  auto br = project(60, 40, 5);
        sprite.drawLine(tl.first, tl.second, (FRAME_W/2)-5, (FRAME_H/2)-2, tunnelColor);
        sprite.drawLine(tr.first, tr.second, (FRAME_W/2)+5, (FRAME_H/2)-2, tunnelColor);
        sprite.drawLine(bl.first, bl.second, (FRAME_W/2)-5, (FRAME_H/2)+2, tunnelColor);
        sprite.drawLine(br.first, br.second, (FRAME_W/2)+5, (FRAME_H/2)+2, tunnelColor);

        // 2. Obstacle Logic
        obstacleZ -= 3.0f; 
        if (obstacleZ <= 1.0f) {
            resetObstacle();
            score++;
        }
        
        auto p1 = project(obsX - 15, obsY - 10, obstacleZ);
        auto p2 = project(obsX + 15, obsY + 10, obstacleZ);
        if (obstacleZ > 1.0f) {
            sprite.drawRect(p1.first, p1.second, p2.first - p1.first, p2.second - p1.second, TFT_RED);
        }

        // 3. Draw Ship (Positioned by Pitch/Roll)
        int shipX = (FRAME_W / 2) + (int)(r * 150.0f);
        int shipY = (FRAME_H / 2) + (int)(p * 150.0f);
        sprite.drawLine(shipX - 10, shipY, shipX + 10, shipY, TFT_CYAN);
        sprite.drawLine(shipX, shipY - 4, shipX, shipY + 2, TFT_WHITE);

        sprite.setTextColor(TFT_GREEN);
        sprite.setCursor(2, 2);
        sprite.printf("Score: %d", score);
    }

    sprite.pushSprite(FRAME_X, FRAME_Y);
}