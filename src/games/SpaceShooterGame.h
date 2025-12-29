#ifndef SPACESHOOTERGAME_H
#define SPACESHOOTERGAME_H

#include "Activity.h" // Updated include

class SpaceShooterGame : public Activity { // Added inheritance
public:
  SpaceShooterGame(TFTHandler& tft, Hardware& hw);
  
  // Mark core methods as override
  void init() override;
  void update() override;
  void draw() override;
  
  void cleanup();
  bool isRunning() const { return running; }

private:
  // REMOVED: TFTHandler& tft; <-- Inherited from Activity
  // REMOVED: Hardware& hw;   <-- Inherited from Activity
  
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