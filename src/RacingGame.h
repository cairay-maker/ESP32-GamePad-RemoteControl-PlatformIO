#ifndef RACINGGAME_H
#define RACINGGAME_H

#include "TFTHandler.h"
#include "Hardware.h"
#include <TFT_eSPI.h>

class RacingGame {
public:
  RacingGame(TFTHandler& tft, Hardware& hw);
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
  float playerX = 0;
  float baseSpeed = 1.0f;
  float currentSpeed = 1.0f;
  float trackPos = 0;
  
  // Strike Logic
  int strikes = 0;
  const int MAX_STRIKES = 5; 
  int score = 0;
  bool isOffRoad = false;
  
  // Effects Logic
  int explosionTimer = 0;
  float explosionX = 0;
  unsigned long lastStrikeTime = 0;
  unsigned long lastSpeedBoostTime = 0;

  struct Obstacle {
    float x;
    float z;
    int animalType; // 0=Cow, 1=Horse, 2=Sheep
    bool active;
    bool passed;
  };

  #define MAX_OBSTACLES 3
  Obstacle obstacles[MAX_OBSTACLES];

  void spawnObstacle();
  void drawCar(int x, int y);
  void drawAnimal(float x, float y, int type, float scale);
  void endGame();
};

#endif