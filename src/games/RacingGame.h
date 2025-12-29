#ifndef RACINGGAME_H
#define RACINGGAME_H

#include "Activity.h"

class RacingGame : public Activity {
public:
  RacingGame(TFTHandler& tft, Hardware& hw);
  
  // Added override keywords to satisfy the Activity base class
  void init() override;
  void update() override;
  void draw() override;
  
  // Note: cleanup() and isRunning() are kept if needed by your menu, 
  // but they are not part of the Activity interface.
  void cleanup();
  bool isRunning() const { return running; }

private:
  // REMOVED: TFTHandler& tft; <-- Inherited from Activity
  // REMOVED: Hardware& hw;   <-- Inherited from Activity

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

  // Obstacle Structure
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