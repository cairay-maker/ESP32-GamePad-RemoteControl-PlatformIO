#include "PongGame.h"
#include <Arduino.h>

PongGame::PongGame(TFTHandler& tftRef, Hardware& hwRef)
    : tft(tftRef), hw(hwRef), sprite(&tft.getTFT())
{
// Leave empty - don't create sprite here
}

void PongGame::init() {
  // Borrow RAM only when starting
  if (!sprite.created()) {
    sprite.createSprite(160, 128);
  }
  running = true;
  score1 = score2 = 0;
  paddle1Y = paddle2Y = 128 / 2 - PADDLE_HEIGHT / 2;
  gameStartTime = millis();
  lastSpeedRamp = gameStartTime;
  speedMultiplier = 1.0f;
  nextServePlayer = 1;
  resetBall();
}

void PongGame::cleanup() {
  // Return RAM to the system
  if (sprite.created()) {
    sprite.deleteSprite();
  }
}

void PongGame::resetBall() {
  if (nextServePlayer == 1) {
    ballX = PADDLE_MARGIN + PADDLE_WIDTH + BALL_RADIUS + 2;
    ballY = paddle1Y + PADDLE_HEIGHT / 2.0f;
    ballVelX = serveVX * speedMultiplier;
  } else {
    ballX = 160 - PADDLE_MARGIN - PADDLE_WIDTH - BALL_RADIUS - 2;
    ballY = paddle2Y + PADDLE_HEIGHT / 2.0f;
    ballVelX = -serveVX * speedMultiplier;
  }
  ballVelY = random(-20, 21) / 10.0f * serveVY * speedMultiplier;
}

void PongGame::update() {
  // Read joysticks
  hw.joyLeft.update();
  hw.joyRight.update();

  float leftY = hw.joyLeft.getMappedY();
  float rightY = hw.joyRight.getMappedY();

  // Move paddles — correct direction
  paddle1Y -= leftY * 5;
  paddle2Y -= rightY * 5;
  paddle1Y = constrain(paddle1Y, 0, 128 - PADDLE_HEIGHT);
  paddle2Y = constrain(paddle2Y, 0, 128 - PADDLE_HEIGHT);

  // Speed ramp: +10% every 10 seconds
  unsigned long currentTime = millis();
  if (currentTime - lastSpeedRamp >= 10000) {
    speedMultiplier *= 1.1f;
    lastSpeedRamp = currentTime;
  }

  // Update ball
  ballX += ballVelX * speedMultiplier;
  ballY += ballVelY * speedMultiplier;

  // Top/bottom bounce
  if (ballY - BALL_RADIUS <= 0 || ballY + BALL_RADIUS >= 128) {
    ballVelY = -ballVelY;
  }

    // Left paddle collision — check if ball is moving LEFT (towards the paddle)
    if (ballVelX < 0 && 
        ballX - BALL_RADIUS <= PADDLE_MARGIN + PADDLE_WIDTH &&
        ballX + BALL_RADIUS >= PADDLE_MARGIN &&
        ballY >= paddle1Y && ballY <= paddle1Y + PADDLE_HEIGHT) {
    
    // Push ball out to the INNER surface of the paddle
    ballX = PADDLE_MARGIN + PADDLE_WIDTH + BALL_RADIUS + 1;

    // Reverse direction to move RIGHT (positive)
    ballVelX = fabs(ballVelX) + 0.2f;

    // Angle based on hit position
    float hitOffset = (ballY - (paddle1Y + PADDLE_HEIGHT / 2.0f)) / (PADDLE_HEIGHT / 2.0f);
    ballVelY = hitOffset * 5.0f * speedMultiplier;
    }
    // Right paddle collision — check if ball is moving RIGHT (towards the paddle)
    if (ballVelX > 0 &&
        ballX + BALL_RADIUS >= 160 - PADDLE_MARGIN - PADDLE_WIDTH &&
        ballX - BALL_RADIUS <= 160 - PADDLE_MARGIN &&
        ballY >= paddle2Y && ballY <= paddle2Y + PADDLE_HEIGHT) {
        
    // Push ball out to the INNER surface
    ballX = 160 - PADDLE_MARGIN - PADDLE_WIDTH - BALL_RADIUS - 1;

    // Reverse direction to move LEFT (negative)
    ballVelX = -(fabs(ballVelX) + 0.2f);

    float hitOffset = (ballY - (paddle2Y + PADDLE_HEIGHT / 2.0f)) / (PADDLE_HEIGHT / 2.0f);
    ballVelY = hitOffset * 5.0f * speedMultiplier;
    }

  // Scoring
  if (ballX < 0) {
    score2++;
    nextServePlayer = 1;
    resetBall();
  }
  if (ballX > 160) {
    score1++;
    nextServePlayer = 2;
    resetBall();
  }

  // Game over
  if (score1 >= WINNING_SCORE || score2 >= WINNING_SCORE) {
    drawGameOver();
    cleanup();
    running = false;
  }
}

void PongGame::draw() {
  sprite.fillSprite(TFT_BLACK);

  // Center line
  for (int y = 0; y < 128; y += 8) {
    sprite.drawPixel(80, y, TFT_WHITE);
  }

  // Paddles
  sprite.fillRect(PADDLE_MARGIN, paddle1Y, PADDLE_WIDTH, PADDLE_HEIGHT, TFT_CYAN);
  sprite.fillRect(160 - PADDLE_MARGIN - PADDLE_WIDTH, paddle2Y, PADDLE_WIDTH, PADDLE_HEIGHT, TFT_MAGENTA);

  // Ball
  sprite.fillCircle((int)ballX, (int)ballY, BALL_RADIUS, TFT_WHITE);

  // Score
  sprite.setTextColor(TFT_WHITE);
  sprite.setTextSize(2);
  sprite.setCursor(40, 10);
  sprite.print(score1);
  sprite.setCursor(110, 10);
  sprite.print(score2);

  // Speed indicator
  sprite.setTextSize(1);
  sprite.setTextColor(TFT_LIGHTGREY);
  sprite.setCursor(5, 120);
  sprite.print("Speed: ");
  sprite.print(speedMultiplier, 2);
  sprite.print("x");

  sprite.pushSprite(0, 0);
}

void PongGame::drawGameOver() {
  tft.clearScreen();
  tft.drawCenteredText("GAME OVER", 40, TFT_RED, 2);
  char buf[32];
  snprintf(buf, sizeof(buf), "P%d WINS!", score1 > score2 ? 1 : 2);
  tft.drawCenteredText(buf, 70, TFT_YELLOW, 2);
  tft.drawCenteredText("Back to menu...", 100, TFT_WHITE, 1);
  delay(3000);
}