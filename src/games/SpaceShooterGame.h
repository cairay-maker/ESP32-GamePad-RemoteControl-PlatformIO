#ifndef SPACESHOOTERGAME_H
#define SPACESHOOTERGAME_H

#include "hal/TFTHandler.h"
#include "hal/Hardware.h"

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
  bool running = true;
  float playerX;
  const int playerY = 110;
  int score = 0;
  unsigned long lastShot = 0;

  struct Bullet { float x, y; bool active; };
  struct Enemy { float x, y, speed; int type, size; uint16_t color; bool active; };
  
  #define MAX_ENEMIES 6
  #define MAX_BULLETS 10
  Bullet bullets[MAX_BULLETS];
  Enemy enemies[MAX_ENEMIES];

  void spawnEnemy();
  void drawPlayer(TFT_eSprite& c, int x, int y);
  void drawEnemy(TFT_eSprite& c, Enemy& e);
  void endGame();
};
#endif