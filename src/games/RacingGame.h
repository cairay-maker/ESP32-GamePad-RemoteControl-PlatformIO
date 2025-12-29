#ifndef RACINGGAME_H
#define RACINGGAME_H

#include "hal/TFTHandler.h"
#include "hal/Hardware.h"

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

  // Game States
  bool running = true;
  bool gameOver = false;

  // Physics and Movement
  float playerX = 0; 
  float trackPos = 0; 
  float baseSpeed = 1.2f; 
  float currentSpeed = 1.0f;

  // Scoring and Collision
  int strikes = 0; 
  int score = 0;
  unsigned long lastStrikeTime = 0;

  // Obstacle Structure (The Farm Animals)
  struct Obstacle { 
    float x, z; 
    int type; // 0: Cow, 1: Pig, 2: Sheep
    bool active; 
  };
  
  #define MAX_OBSTACLES 3
  Obstacle obstacles[MAX_OBSTACLES];

  // Internal Helpers
  void spawnObstacle();
  void drawCar(TFT_eSprite& c, int x, int y);
  void drawAnimal(TFT_eSprite& c, float x, float y, int type, float scale);
  void endGame();
};

#endif