#include "games/SpaceShooterGame.h"
#include <Arduino.h>

SpaceShooterGame::SpaceShooterGame(TFTHandler& tftRef, Hardware& hwRef) 
    : Activity(tftRef, hwRef) {} // Correctly passes both to Activity base class

void SpaceShooterGame::init() {
  running = true; 
  playerX = 74; 
  score = 0;
  for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
  for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
}

void SpaceShooterGame::spawnEnemy() {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (!enemies[i].active) {
      enemies[i].x = random(10, 140);
      enemies[i].y = -15;
      enemies[i].active = true;
      enemies[i].type = random(0, 3); 
      
      if (enemies[i].type == 0) { 
          enemies[i].speed = 2.0f; enemies[i].size = 6; enemies[i].color = TFT_RED;
      } else if (enemies[i].type == 1) { 
          enemies[i].speed = 1.2f; enemies[i].size = 10; enemies[i].color = TFT_MAGENTA;
      } else { 
          enemies[i].speed = 0.7f; enemies[i].size = 14; enemies[i].color = 0xFBE0; 
      }
      break;
    }
  }
}

void SpaceShooterGame::update() {
  // Uses 'hw' inherited from Activity
  playerX = constrain(playerX + hw.state.joyLX * 4.5f, 0, 160 - 12);
  
  if (hw.state.joyRY > 0.5f && millis() - lastShot > 250) {
    for (int i = 0; i < MAX_BULLETS; i++) {
      if (!bullets[i].active) {
        bullets[i].x = playerX + 6; 
        bullets[i].y = playerY;
        bullets[i].active = true; 
        lastShot = millis(); 
        break;
      }
    }
  }

  if (random(0, 100) < 4) spawnEnemy();

  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i].active) {
      bullets[i].y -= 5;
      if (bullets[i].y < -5) bullets[i].active = false;
    }
  }

  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].active) {
      enemies[i].y += enemies[i].speed;

      if (enemies[i].y + enemies[i].size > playerY && 
          enemies[i].x + enemies[i].size > playerX && 
          enemies[i].x < playerX + 12) {
          endGame();
      }

      for (int j = 0; j < MAX_BULLETS; j++) {
        if (bullets[j].active) {
          if (bullets[j].x > enemies[i].x && bullets[j].x < enemies[i].x + enemies[i].size &&
              bullets[j].y > enemies[i].y && bullets[j].y < enemies[i].y + enemies[i].size) {
            enemies[i].active = false;
            bullets[j].active = false;
            score += 10;
          }
        }
      }

      if (enemies[i].y > 128) enemies[i].active = false;
    }
  }
}

void SpaceShooterGame::draw() {
  // Uses 'tft' inherited from Activity
  TFT_eSprite& c = tft.canvas;
  c.fillSprite(TFT_BLACK);
  
  for(int i=0; i<10; i++) c.drawPixel(random(160), random(128), TFT_DARKGREY);

  drawPlayer(c, (int)playerX, playerY);
  
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i].active) c.fillCircle(bullets[i].x, bullets[i].y, 2, TFT_YELLOW);
  }

  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].active) drawEnemy(c, enemies[i]);
  }

  c.setTextColor(TFT_WHITE);
  c.setCursor(5, 5);
  c.printf("SCORE: %d", score);
}

void SpaceShooterGame::drawPlayer(TFT_eSprite& c, int x, int y) {
  c.fillTriangle(x, y + 12, x + 6, y, x + 12, y + 12, TFT_CYAN); 
  c.fillRect(x + 5, y + 8, 2, 4, TFT_WHITE); 
  c.drawTriangle(x, y + 12, x + 6, y, x + 12, y + 12, TFT_WHITE); 
}

void SpaceShooterGame::drawEnemy(TFT_eSprite& c, Enemy& e) {
  if (e.type == 0) { 
    c.fillTriangle(e.x + e.size/2, e.y, e.x, e.y + e.size, e.x + e.size, e.y + e.size, e.color);
  } else if (e.type == 1) { 
    c.fillRect((int)e.x, (int)e.y, e.size, e.size, e.color);
    c.drawRect((int)e.x, (int)e.y, e.size, e.size, TFT_WHITE);
  } else { 
    c.fillCircle(e.x + e.size/2, e.y + e.size/2, e.size/2, e.color);
    c.drawFastHLine(e.x - 2, e.y + e.size/2, e.size + 4, TFT_WHITE);
  }
}

void SpaceShooterGame::endGame() {
  running = false;
}

void SpaceShooterGame::cleanup() {
}