#ifndef PONGGAME_H
#define PONGGAME_H

#include "hal/TFTHandler.h"
#include "hal/Hardware.h"

class PongGame {
public:
  PongGame(TFTHandler& tft, Hardware& hw);
  void init();
  void update();
  void draw();
  void cleanup();
  bool isRunning() const { return running; }

private:
  TFTHandler& tft;
  Hardware& hw;
  bool running = true;
  bool gameOver = false;

  #define PADDLE_WIDTH 4
  #define PADDLE_HEIGHT 24
  #define BALL_RADIUS 3
  #define PADDLE_MARGIN 5

  const int winningScore = 5; // Winning score setting

  float ballX, ballY, ballVelX, ballVelY;
  float paddle1Y, paddle2Y;
  int score1 = 0, score2 = 0;
  float speedMultiplier = 1.0f;
  int nextServePlayer = 1;

  void resetBall();
};
#endif