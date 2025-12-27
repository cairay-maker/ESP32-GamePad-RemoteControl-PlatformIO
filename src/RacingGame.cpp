#include "RacingGame.h"
#include <Arduino.h>

RacingGame::RacingGame(TFTHandler& tftRef, Hardware& hwRef)
    : tft(tftRef), hw(hwRef), sprite(&tft.getTFT()) {}

void RacingGame::init() {
  if (!sprite.created()) sprite.createSprite(160, 128);
  running = true;
  playerX = 0;
  baseSpeed = 1.0f;
  trackPos = 0;
  strikes = 0;
  score = 0;
  explosionTimer = 0;
  lastSpeedBoostTime = millis();
  lastStrikeTime = 0;
  for(int i=0; i<MAX_OBSTACLES; i++) {
    obstacles[i].active = false;
    obstacles[i].passed = false;
  }
}

void RacingGame::cleanup() {
  if (sprite.created()) sprite.deleteSprite();
}

void RacingGame::spawnObstacle() {
  for(int i=0; i<MAX_OBSTACLES; i++) {
    if(!obstacles[i].active) {
      obstacles[i].x = (random(-70, 71) / 100.0f);
      obstacles[i].z = 100.0f;
      obstacles[i].animalType = random(0, 3);
      obstacles[i].active = true;
      obstacles[i].passed = false;
      break;
    }
  }
}

// 7-Pointed Red Star Splash Helper
void drawStarSplash(TFT_eSprite &s, int x, int y, int size) {
  int points = 7;
  float angleStep = (2.0f * PI) / points;
  for (int i = 0; i < points; i++) {
    float angle = i * angleStep;
    // Outer point
    int x1 = x + cos(angle) * size;
    int y1 = y + sin(angle) * size;
    // Inner point (at 1/3 size for sharp points)
    int x2 = x + cos(angle + angleStep/2) * (size/3);
    int y2 = y + sin(angle + angleStep/2) * (size/3);
    // Next outer point
    int x3 = x + cos(angle + angleStep) * size;
    int y3 = y + sin(angle + angleStep) * size;
    
    s.fillTriangle(x, y, x1, y1, x2, y2, TFT_RED);
    s.fillTriangle(x, y, x2, y2, x3, y3, TFT_RED);
  }
}

void RacingGame::update() {
  hw.joyLeft.update();
  hw.joyRight.update();

  if (millis() - lastSpeedBoostTime >= 10000) {
    baseSpeed *= 1.10f;
    lastSpeedBoostTime = millis();
  }

  float boost = hw.joyRight.getMappedY();
  currentSpeed = baseSpeed + (boost > 0.2f ? boost * 2.0f : 0);
  playerX += hw.joyLeft.getMappedX() * 0.07f;
  trackPos += currentSpeed;

  if (random(0, 100) < 3) spawnObstacle();

  for(int i=0; i<MAX_OBSTACLES; i++) {
    if(obstacles[i].active) {
      obstacles[i].z -= currentSpeed;

      // TRIGGER EARLY: Collision check at z < 15
      if (obstacles[i].z < 15 && obstacles[i].z > -5) {
        if (abs(playerX - obstacles[i].x) < 0.35f) {
           if (millis() - lastStrikeTime > 1550) {
             strikes++;
             lastStrikeTime = millis();
             obstacles[i].active = false;
             
             // Setup Star Splash
             explosionTimer = 12; 
             explosionX = 80 + (playerX * 60);
             
             if(strikes >= MAX_STRIKES) endGame();
           }
        } else if (!obstacles[i].passed && obstacles[i].z < 0) {
          score += 10;
          obstacles[i].passed = true;
        }
      }
      if (obstacles[i].z < -10) obstacles[i].active = false;
    }
  }

  if (abs(playerX) > 0.85f) {
    isOffRoad = true;
    if (millis() - lastStrikeTime > 1500) {
      strikes++;
      lastStrikeTime = millis();
      explosionTimer = 8;
      explosionX = 80 + (playerX * 60);
      if (strikes >= MAX_STRIKES) endGame();
    }
  } else isOffRoad = false;
}

void RacingGame::drawAnimal(float x, float y, int type, float scale) {
  int s = (int)(scale * 12);
  switch(type) {
    case 0: // Cow
      sprite.fillRoundRect(x, y, s+8, s+4, 2, TFT_WHITE);
      sprite.fillCircle(x + s + 6, y + 2, s/2 + 1, TFT_WHITE);
      sprite.fillCircle(x + 3, y + 3, s/4 + 1, TFT_BLACK);
      break;
    case 1: // Horse
      sprite.fillRoundRect(x, y, s+10, s+4, 1, 0x9300);
      sprite.fillRect(x + s + 7, y - 3, s/3 + 2, s+2, 0x9300);
      break;
    case 2: // Sheep
      sprite.fillCircle(x, y, s + 3, TFT_WHITE);
      sprite.fillCircle(x + 5, y, s + 3, TFT_WHITE);
      sprite.fillCircle(x + 7, y + 2, s/3 + 2, TFT_BLACK);
      break;
  }
}

void RacingGame::draw() {
  sprite.fillSprite(TFT_SKYBLUE);

  // Draw Road
  for (int y = 0; y < 64; y++) {
    float perspective = (float)y / 64.0f;
    float roadWidth = 30 + (perspective * 110);
    float curveOffset = sin(trackPos * 0.02f) * (perspective * 40);
    float centerX = 80 + curveOffset;
    int screenY = 64 + y;
    uint16_t grassColor = ((int)(trackPos + y*2) % 40 < 20) ? 0x03E0 : 0x02E0; 
    sprite.drawFastHLine(0, screenY, 160, grassColor);
    sprite.drawFastHLine(centerX - roadWidth/2, screenY, roadWidth, 0x4208);
  }

  // Draw Animals
  for(int i=0; i<MAX_OBSTACLES; i++) {
    if(obstacles[i].active) {
      float p = (100.0f - obstacles[i].z) / 100.0f;
      if(p > 0) {
        float curveAtZ = sin((trackPos + obstacles[i].z) * 0.02f) * (p * 40);
        float obsX = 80 + curveAtZ + (obstacles[i].x * (30 + p * 110));
        float obsY = 64 + (p * 64);
        drawAnimal(obsX, obsY, obstacles[i].animalType, p);
      }
    }
  }

  // Draw Car
  drawCar(80 + (playerX * 60) - 15, 100);

  // Draw Star Splash Effect
  if (explosionTimer > 0) {
    drawStarSplash(sprite, explosionX, 105, 15 + random(5));
    explosionTimer--;
  }

  // UI (Always on Top)
  sprite.setTextColor(TFT_WHITE, TFT_SKYBLUE);
  sprite.setCursor(5, 5);
  sprite.printf("SCORE: %d  SPD: %.1fx", score, baseSpeed);
  sprite.setCursor(5, 15);
  sprite.print("LIFE: ");
  for(int i=0; i<MAX_STRIKES; i++) {
    sprite.setTextColor(i < strikes ? TFT_RED : TFT_GREEN, TFT_SKYBLUE);
    sprite.print(i < strikes ? "X " : "O ");
  }

  sprite.pushSprite(0, 0);
}

void RacingGame::drawCar(int x, int y) {
  sprite.fillRoundRect(x, y + 4, 30, 12, 3, TFT_BLUE);
  sprite.fillRoundRect(x + 5, y, 20, 8, 4, TFT_SKYBLUE);
  sprite.fillRoundRect(x - 2, y + 10, 6, 8, 2, TFT_BLACK);
  sprite.fillRoundRect(x + 26, y + 10, 6, 8, 2, TFT_BLACK);
}

void RacingGame::endGame() {
  tft.getTFT().fillScreen(TFT_BLACK);
  tft.drawCenteredText("TOTALED!", 50, TFT_RED, 2);
  delay(2000);
  cleanup();
  running = false;
}