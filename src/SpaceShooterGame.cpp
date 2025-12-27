#include "SpaceShooterGame.h"
#include <Arduino.h>

SpaceShooterGame::SpaceShooterGame(TFTHandler& tftRef, Hardware& hwRef)
    : tft(tftRef), hw(hwRef), sprite(&tft.getTFT()) {}

void SpaceShooterGame::init() {
  if (!sprite.created()) sprite.createSprite(160, 128);
  running = true;
  playerX = 74;
  score = 0;
  lastShot = millis();
  lastEnemySpawn = millis();
  for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
  for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
}

void SpaceShooterGame::cleanup() {
  if (sprite.created()) sprite.deleteSprite();
}

void SpaceShooterGame::update() {
  hw.joyLeft.update();
  hw.joyRight.update();
  playerX += hw.joyLeft.getMappedX() * 5;
  playerX = constrain(playerX, 0, 160 - PLAYER_WIDTH);

  if (hw.joyRight.getMappedY() > 0.5f && millis() - lastShot > 250) {
    for (int i = 0; i < MAX_BULLETS; i++) {
      if (!bullets[i].active) {
        bullets[i].x = playerX + (PLAYER_WIDTH / 2);
        bullets[i].y = playerY;
        bullets[i].active = true;
        lastShot = millis();
        break;
      }
    }
  }

  if (millis() - lastEnemySpawn > 1200) spawnEnemy();

  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i].active) {
      bullets[i].y -= 5;
      if (bullets[i].y < 0) bullets[i].active = false;
      for (int j = 0; j < MAX_ENEMIES; j++) {
        if (enemies[j].active && abs(bullets[i].x - (enemies[j].x + enemies[j].size/2)) < enemies[j].size && abs(bullets[i].y - (enemies[j].y + enemies[j].size/2)) < enemies[j].size) {
          bullets[i].active = false;
          enemies[j].active = false;
          score += (enemies[j].type + 1) * 10;
        }
      }
    }
  }

  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].active) {
      enemies[i].y += enemies[i].speed;
      if (enemies[i].y > 128) enemies[i].active = false;
      if (abs(playerX + 6 - (enemies[i].x + enemies[i].size/2)) < 8 && abs(playerY + 6 - (enemies[i].y + enemies[i].size/2)) < 8) {
        endGame();
      }
    }
  }
}

void SpaceShooterGame::spawnEnemy() {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (!enemies[i].active) {
      enemies[i].type = random(0, 3);
      enemies[i].y = -15;
      enemies[i].x = random(10, 140);
      enemies[i].active = true;
      if (enemies[i].type == 0) { enemies[i].size = 12; enemies[i].speed = 0.7f; enemies[i].color = TFT_RED; }
      else if (enemies[i].type == 1) { enemies[i].size = 9; enemies[i].speed = 1.3f; enemies[i].color = TFT_MAGENTA; }
      else { enemies[i].size = 8; enemies[i].speed = 2.2f; enemies[i].color = TFT_ORANGE; }
      lastEnemySpawn = millis();
      break;
    }
  }
}

void SpaceShooterGame::draw() {
  sprite.fillSprite(TFT_BLACK);
  for(int i=0; i<8; i++) sprite.drawPixel(random(160), random(128), 0x7BEF);
  drawPlayer((int)playerX, playerY);
  for (int i = 0; i < MAX_BULLETS; i++) if (bullets[i].active) sprite.fillCircle(bullets[i].x, bullets[i].y, 2, TFT_YELLOW);
  for (int i = 0; i < MAX_ENEMIES; i++) if (enemies[i].active) drawEnemy(enemies[i]);
  sprite.setTextColor(TFT_WHITE);
  sprite.setCursor(5, 5);
  sprite.printf("SCORE: %d", score);
  sprite.pushSprite(0, 0);
}

void SpaceShooterGame::drawPlayer(int x, int y) {
  sprite.fillTriangle(x, y + 12, x + 6, y, x + 12, y + 12, TFT_CYAN);
  sprite.fillRect(x + 5, y + 4, 2, 8, TFT_WHITE);
  sprite.fillRect(x + 4, y + 10, 4, 3, TFT_RED);
}

void SpaceShooterGame::drawEnemy(Enemy& e) {
  if (e.type == 0) sprite.fillSmoothRoundRect(e.x, e.y, e.size, e.size, 2, e.color);
  else if (e.type == 1) { sprite.fillRect(e.x, e.y, e.size, e.size, e.color); sprite.drawRect(e.x-1, e.y-1, e.size+2, e.size+2, TFT_WHITE); }
  else { 
    sprite.drawLine(e.x, e.y, e.x + e.size, e.y + e.size, e.color); 
    sprite.drawLine(e.x + e.size, e.y, e.x, e.y + e.size, e.color); 
  }
}

void SpaceShooterGame::endGame() {
  tft.getTFT().fillScreen(TFT_BLACK);
  tft.drawCenteredText("GAME OVER", 50, TFT_RED, 2);
  delay(2000);
  cleanup();
  running = false;
}