#include "games/PongGame.h"
#include <Arduino.h>

PongGame::PongGame(TFTHandler& tftRef, Hardware& hwRef) 
    : tft(tftRef), hw(hwRef) {}

void PongGame::init() {
  running = true;
  gameOver = false;
  score1 = 0;
  score2 = 0;
  paddle1Y = 64 - PADDLE_HEIGHT / 2;
  paddle2Y = 64 - PADDLE_HEIGHT / 2;
  speedMultiplier = 1.0f;
  nextServePlayer = 1;
  resetBall();
}

void PongGame::resetBall() {
  // Serve from the side of the player who was just scored on
  ballX = (nextServePlayer == 1) ? 30 : 130;
  ballY = 64;
  
  // Initial velocity
  ballVelX = (nextServePlayer == 1) ? 2.0f : -2.0f;
  ballVelY = (random(-15, 16) / 10.0f);
  
  // Reset speed ramp for the new round
  speedMultiplier = 1.0f;
}

void PongGame::update() {
  // Handle Game Over state
  if (gameOver) {
    // Check bit 4 for the SELECT key based on your hardware bit-packing
    if (hw.state.buttons & (1 << 4)) {
      init();
    }
    return;
  }

  // Paddle Movement: Left Joystick for P1, Right Joystick for P2
  paddle1Y = constrain(paddle1Y - (hw.state.joyLY * 5.0f), 0, 128 - PADDLE_HEIGHT);
  paddle2Y = constrain(paddle2Y - (hw.state.joyRY * 5.0f), 0, 128 - PADDLE_HEIGHT);

  // Ball Physics with Speed Ramp
  ballX += ballVelX * speedMultiplier;
  ballY += ballVelY * speedMultiplier;

  // Ceiling and Floor Bounce
  if (ballY <= BALL_RADIUS || ballY >= 128 - BALL_RADIUS) {
    ballVelY *= -1.0f;
    ballY = constrain(ballY, (float)BALL_RADIUS, (float)(128 - BALL_RADIUS));
  }

  // Paddle 1 Collision (Left)
  if (ballVelX < 0 && ballX < PADDLE_MARGIN + PADDLE_WIDTH) {
    if (ballY >= paddle1Y && ballY <= paddle1Y + PADDLE_HEIGHT) {
      ballVelX = fabs(ballVelX);
      ballX = PADDLE_MARGIN + PADDLE_WIDTH + 1;
      
      // Speed Ramp: Increase speed by 10% on every hit
      speedMultiplier += 0.10f;
      
      // Add "English" angle based on where ball hits paddle
      ballVelY = (ballY - (paddle1Y + PADDLE_HEIGHT / 2.0f)) * 0.25f;
    }
  }

  // Paddle 2 Collision (Right)
  if (ballVelX > 0 && ballX > 160 - PADDLE_MARGIN - PADDLE_WIDTH) {
    if (ballY >= paddle2Y && ballY <= paddle2Y + PADDLE_HEIGHT) {
      ballVelX = -fabs(ballVelX);
      ballX = 160 - PADDLE_MARGIN - PADDLE_WIDTH - 1;
      
      // Speed Ramp: Increase speed by 10% on every hit
      speedMultiplier += 0.10f;
      
      // Add "English" angle based on where ball hits paddle
      ballVelY = (ballY - (paddle2Y + PADDLE_HEIGHT / 2.0f)) * 0.25f;
    }
  }

  // Scoring Logic
  if (ballX < 0) {
    score2++;
    nextServePlayer = 1;
    if (score2 >= winningScore) gameOver = true;
    else resetBall();
  } else if (ballX > 160) {
    score1++;
    nextServePlayer = 2;
    if (score1 >= winningScore) gameOver = true;
    else resetBall();
  }
}

void PongGame::draw() {
  TFT_eSprite& c = tft.canvas;
  c.fillSprite(TFT_BLACK);

  // Draw Center Line
  for (int y = 0; y < 128; y += 8) {
    c.drawFastVLine(80, y, 4, 0x3186); // Dim grey
  }

  // Draw Scores (Restore missing display)
  c.setTextColor(TFT_WHITE);
  c.setTextSize(2);
  c.setCursor(40, 10);
  c.print(score1);
  c.setCursor(110, 10);
  c.print(score2);

  if (gameOver) {
    c.setTextSize(2);
    c.setCursor(35, 55);
    if (score1 >= winningScore) c.print("P1 WINS!");
    else c.print("P2 WINS!");
    
    c.setTextSize(1);
    c.setCursor(30, 85);
    c.print("PRESS SELECT TO RESTART");
    return;
  }

  // Draw Paddles
  c.fillRect(PADDLE_MARGIN, (int)paddle1Y, PADDLE_WIDTH, PADDLE_HEIGHT, TFT_CYAN);
  c.fillRect(160 - PADDLE_MARGIN - PADDLE_WIDTH, (int)paddle2Y, PADDLE_WIDTH, PADDLE_HEIGHT, TFT_MAGENTA);

  // Draw Ball
  c.fillCircle((int)ballX, (int)ballY, BALL_RADIUS, TFT_WHITE);

  // Speed UI (Subtle feedback)
  c.setTextSize(1);
  c.setTextColor(TFT_DARKGREY);
  c.setCursor(72, 115);
  c.printf("x%.1f", speedMultiplier);
}

void PongGame::cleanup() {}