#ifndef PONGGAME_H
#define PONGGAME_H

#include "Activity.h" // Crucial: This replaces individual hal includes

class PongGame : public Activity {
public:
  PongGame(TFTHandler& tft, Hardware& hw);
  void init() override;
  void update() override;
  void draw() override;
  
  // Note: cleanup() was not in the Activity base class, 
  // we use exit() instead if you want to follow the pattern.
  void exit() override { running = false; }

private:
  // DELETE: TFTHandler& tft;  <-- Already in Activity.h
  // DELETE: Hardware& hw;    <-- Already in Activity.h
  
  bool running = true;
  bool gameOver = false;

  #define PADDLE_WIDTH 4
  #define PADDLE_HEIGHT 24
  #define BALL_RADIUS 3
  #define PADDLE_MARGIN 5

  const int winningScore = 5;

  float ballX, ballY, ballVelX, ballVelY;
  float paddle1Y, paddle2Y;
  int score1 = 0, score2 = 0;
  float speedMultiplier = 1.0f;
  int nextServePlayer = 1;

  void resetBall();
};
#endif