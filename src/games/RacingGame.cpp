#include "games/RacingGame.h"
#include <Arduino.h>

RacingGame::RacingGame(TFTHandler& tftRef, Hardware& hwRef) 
    : tft(tftRef), hw(hwRef) {}

void RacingGame::init() {
  running = true;
  gameOver = false;
  playerX = 0; 
  baseSpeed = 1.2f; 
  trackPos = 0; 
  strikes = 0; 
  score = 0;
  for(int i=0; i<MAX_OBSTACLES; i++) obstacles[i].active = false;
}

void RacingGame::spawnObstacle() {
  for(int i=0; i<MAX_OBSTACLES; i++) {
    if(!obstacles[i].active) {
      obstacles[i].x = (random(-70, 71) / 100.0f);
      obstacles[i].z = 100.0f;
      obstacles[i].type = random(0, 3); 
      obstacles[i].active = true;
      break;
    }
  }
}

void RacingGame::update() {
  if (gameOver) {
    if (hw.state.buttons & (1 << 4)) init(); 
    return;
  }

  // Steering: Right Joystick X
  playerX = constrain(playerX + (hw.state.joyRX * 0.12f), -1.0f, 1.0f);

  // Speed & Boost
  float boost = hw.state.joyRY; 
  currentSpeed = baseSpeed + (boost > 0.1f ? boost * 3.0f : 0);
  trackPos += currentSpeed;

  if (random(0, 100) < 4) spawnObstacle();

  for(int i=0; i<MAX_OBSTACLES; i++) {
    if(obstacles[i].active) {
      obstacles[i].z -= currentSpeed;
      
      if (obstacles[i].z < 15 && obstacles[i].z > 0) {
        if (abs(playerX - obstacles[i].x) < 0.35f) {
           if (millis() - lastStrikeTime > 1500) {
             strikes++;
             lastStrikeTime = millis();
             obstacles[i].active = false; 
             if(strikes >= 3) gameOver = true;
           }
        }
      }
      
      if (obstacles[i].z < -5) {
        obstacles[i].active = false;
        score += 10;
      }
    }
  }
}

void RacingGame::draw() {
  TFT_eSprite& c = tft.canvas;
  
  int shakeX = 0, shakeY = 0;
  if (millis() - lastStrikeTime < 300) {
    shakeX = random(-5, 6);
    shakeY = random(-5, 6);
  }

  c.fillRect(0, 0, 160, 64, 0x3CFD); // Sky

  // Draw Ground & Road
  for (int y = 0; y < 64; y++) {
    float p = (float)y / 64.0f;
    float w = 30 + (p * 110); 
    float curve = sin((trackPos + y) * 0.02f) * (p * 30);
    int roadX = 80 + curve + shakeX;
    int screenY = 64 + y + shakeY;

    c.drawFastHLine(0, screenY, 160, 0x03E0); // Grass
    c.drawFastHLine(roadX - w/2, screenY, (int)w, 0x4208); // Road
    c.drawPixel(roadX - w/2, screenY, TFT_WHITE); // Shoulders
    c.drawPixel(roadX + w/2, screenY, TFT_WHITE);
  }

  // Draw Animals
  for(int i=0; i<MAX_OBSTACLES; i++) {
    if(obstacles[i].active) {
      float p = (100.0f - obstacles[i].z) / 100.0f; 
      if(p > 0) {
        float curve = sin((trackPos + obstacles[i].z) * 0.02f) * (p * 30);
        float ox = 80 + curve + (obstacles[i].x * (30 + p * 110)) + shakeX;
        float oy = 64 + (p * 64) + shakeY;
        drawAnimal(c, ox, oy, obstacles[i].type, p);
      }
    }
  }

  // RED Explosive Effect
  if (millis() - lastStrikeTime < 600) {
      for(int p=0; p<15; p++) {
          c.fillCircle(80 + (playerX * 60) + random(-20, 21), 105 + random(-15, 10), random(1, 5), TFT_RED);
      }
  }

  drawCar(c, 80 + (playerX * 60) + shakeX, 115 + shakeY);
  
  // HUD
  c.setTextColor(TFT_YELLOW);
  c.setTextSize(1);
  c.setCursor(5, 5);
  c.printf("SCORE: %d", score);
  
  for(int i=0; i<(3-strikes); i++) {
      c.fillCircle(150 - (i*12), 10, 4, 0xF800); // Red lives
  }

  if (gameOver) {
    c.fillRect(20, 45, 120, 40, TFT_BLACK);
    c.drawRect(20, 45, 120, 40, TFT_WHITE);
    c.setTextColor(TFT_RED);
    c.setCursor(55, 52);
    c.print("CRASH!");
    c.setTextColor(TFT_WHITE);
    c.setCursor(30, 68);
    c.print("SELECT TO RESTART");
  }
}

// Sleek Sporty Racing Car (Narrow Front)
void RacingGame::drawCar(TFT_eSprite& c, int x, int y) {
  // Main chassis (Triangle shape for narrow front)
  // x is the center of the car
  c.fillTriangle(x, y - 20, x - 15, y, x + 15, y, TFT_ORANGE);
  
  // Cockpit/Windshield
  c.fillTriangle(x, y - 12, x - 8, y - 2, x + 8, y - 2, 0x7BEF);
  
  // Rear Spoiler
  c.drawFastHLine(x - 15, y, 30, 0x8000); // Darker orange/red base
  c.fillRect(x - 16, y - 4, 4, 6, TFT_ORANGE);
  c.fillRect(x + 12, y - 4, 4, 6, TFT_ORANGE);
  c.drawFastHLine(x - 16, y - 4, 32, TFT_ORANGE);

  // Wheels (Hidden under body mostly)
  c.fillRect(x - 17, y - 10, 4, 8, TFT_BLACK);
  c.fillRect(x + 13, y - 10, 4, 8, TFT_BLACK);
}

void RacingGame::drawAnimal(TFT_eSprite& c, float x, float y, int type, float scale) {
  int bodyW = (int)(scale * 20) + 4;
  int bodyH = (int)(scale * 12) + 3;
  if (type == 0) { // COW
    c.fillRoundRect(x - bodyW/2, y - bodyH, bodyW, bodyH, 2, TFT_WHITE);
    c.fillCircle(x - bodyW/4, y - bodyH + 3, 2, TFT_BLACK); 
  } else if (type == 1) { // PIG
    c.fillRoundRect(x - bodyW/2, y - bodyH, bodyW, bodyH, 2, 0xFC18); 
  } else { // SHEEP
    c.fillCircle(x, y - bodyH/2, bodyW/2, 0xCE79);
    c.fillCircle(x - bodyW/2, y - bodyH/2, 3, TFT_BLACK); 
  }
}

void RacingGame::endGame() { gameOver = true; }
void RacingGame::cleanup() {}