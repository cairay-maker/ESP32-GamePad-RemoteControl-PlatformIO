#include "games/StarshipGame.h"
#include <Arduino.h>

StarshipGame::StarshipGame(TFTHandler& tftRef, Hardware& hwRef) 
    : tft(tftRef), hw(hwRef) {}

void StarshipGame::init() {
    running = true;
    score = 0;
    speed = 2.5f;
    lastSpeedUp = millis();
    for(int i = 0; i < MAX_STARSHIP_OBS; i++) {
        spawnObstacle(i);
        obs[i].z = 100.0f + (i * 40.0f); 
    }
}

void StarshipGame::spawnObstacle(int i) {
    obs[i].x = (float)random(-55, 56);
    obs[i].y = (float)random(-55, 56);
    obs[i].z = 150.0f;
    obs[i].active = true;
}

StarshipGame::Point2D StarshipGame::project(float x, float y, float z) {
    Point2D p;
    if (z < 0.1f) z = 0.1f;
    float factor = 120.0f / z; 
    p.x = 80 + (int)(x * factor);
    p.y = 64 + (int)(y * factor);
    p.size = (int)(factor * 4);
    return p;
}

void StarshipGame::update() {
    if (millis() - lastSpeedUp > 5000) {
        speed += 0.2f;
        lastSpeedUp = millis();
    }

    // GET IMU DATA DIRECTLY
    // Roll (tilt left/right) and Pitch (tilt forward/back)
    // We constrain the input so a ~45 degree tilt gives full screen movement
    float r = hw.imu.getRoll() * (PI / 180.0f);  
    float p = hw.imu.getPitch() * (PI / 180.0f); 

    // shipWorld coordinates used for collision detection
    float shipWorldX = r * 60.0f; 
    float shipWorldY = p * 60.0f;

    for (int i = 0; i < MAX_STARSHIP_OBS; i++) {
        obs[i].z -= speed;
        
        // Collision detection at the "near" plane
        if (obs[i].z < 10.0f && obs[i].z > 0.0f) {
            float dx = obs[i].x - shipWorldX;
            float dy = obs[i].y - shipWorldY;
            if (sqrt(dx*dx + dy*dy) < 16.0f) endGame();
        }

        if (obs[i].z <= 0) {
            spawnObstacle(i);
            score += 10;
        }
    }
}

void StarshipGame::draw() {
    TFT_eSprite& c = tft.canvas;
    c.fillSprite(TFT_BLACK);

    // Static Star Field / Tunnel Lines
    c.drawLine(0, 0, 160, 128, 0x18C3);
    c.drawLine(160, 0, 0, 128, 0x18C3);

    for (int i = 0; i < MAX_STARSHIP_OBS; i++) {
        Point2D pt = project(obs[i].x, obs[i].y, obs[i].z);
        if (pt.x > -pt.size && pt.x < 160 + pt.size) {
            uint16_t color = (obs[i].z < 40.0f) ? TFT_RED : TFT_GREEN;
            c.drawRect(pt.x - pt.size, pt.y - pt.size, pt.size * 2, pt.size * 2, color);
        }
    }

    // RE-CALCULATE RETICLE POSITION FOR DRAWING
    float r = hw.imu.getRoll() * (PI / 180.0f);
    float p = hw.imu.getPitch() * (PI / 180.0f);

    int sx = 80 + (int)(r * 90); // Multiplier adjusted for screen width
    int sy = 64 + (int)(p * 80); // Multiplier adjusted for screen height

    // Draw Crosshair
    c.drawCircle(sx, sy, 12, TFT_CYAN);
    c.drawFastHLine(sx - 18, sy, 36, TFT_WHITE);
    c.drawFastVLine(sx, sy - 18, 36, TFT_WHITE);
    c.drawPixel(sx, sy, TFT_WHITE);

    // Score
    c.setTextColor(TFT_YELLOW);
    c.setCursor(5, 5);
    c.printf("WARP: %d", score);
}

void StarshipGame::endGame() { 
    running = false; 
}

void StarshipGame::cleanup() {}