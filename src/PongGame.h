#ifndef PONGGAME_H
#define PONGGAME_H

#include "TFTHandler.h"
#include "Hardware.h"
#include <TFT_eSPI.h>  // For TFT_eSprite

class PongGame {
public:
  PongGame(TFTHandler& tft, Hardware& hw);
  void init();
  void update();
  void draw();
  void cleanup(); // <--- Add this line
  bool isRunning() const { return running; }

private:
  TFTHandler& tft;
  Hardware& hw;

  bool running = true;

  // Pong constants
  #define PADDLE_WIDTH 4
  #define PADDLE_HEIGHT 24
  #define BALL_RADIUS 3
  #define PADDLE_MARGIN 5
  #define WINNING_SCORE 3

  float ballX, ballY, ballVelX, ballVelY;
  int paddle1Y, paddle2Y;
  int score1 = 0, score2 = 0;
  float serveVX = 3.0f, serveVY = 2.0f;

  // Speed ramp and timing
  unsigned long gameStartTime = 0;
  unsigned long lastSpeedRamp = 0;
  float speedMultiplier = 1.0f;

  // Turn-based serve
  int nextServePlayer = 1;  // 1 = left player, 2 = right player

  // Sprite for flicker-free drawing
  TFT_eSprite sprite;

  void resetBall();
  void drawGameOver();
};

#endif