#ifndef SPACESHOOTERGAME_H
#define SPACESHOOTERGAME_H

#include "TFTHandler.h"
#include "Hardware.h"
#include <TFT_eSPI.h>

class SpaceShooterGame {
public:
  SpaceShooterGame(TFTHandler& tft, Hardware& hw);
  void init();
  void update();
  void draw();
  void cleanup();
  bool isRunning() const { return running; }

private:
  TFTHandler& tft;
  Hardware& hw;
  TFT_eSprite sprite;

  bool running = true;
  #define PLAYER_WIDTH 12
  #define PLAYER_HEIGHT 12
  #define MAX_ENEMIES 6
  #define MAX_BULLETS 10

  float playerX;
  const int playerY = 110;
  int score = 0;

  struct Bullet { float x, y; bool active; };
  Bullet bullets[MAX_BULLETS];

  struct Enemy {
    float x, y, speed;
    int type, size;
    uint16_t color;
    bool active;
  };
  Enemy enemies[MAX_ENEMIES];

  unsigned long lastShot = 0;
  unsigned long lastEnemySpawn = 0;

  void spawnEnemy();
  void drawPlayer(int x, int y);
  void drawEnemy(Enemy& e);
  void endGame();
};

#endif