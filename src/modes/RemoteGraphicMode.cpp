#include "modes/RemoteGraphicMode.h"
#include <Arduino.h>
#include <math.h>

static const int JOYSTICK_DOT = 3;
static const uint16_t JOYSTICK_COLOR = TFT_RED;
static const uint16_t ENCODER_COLOR = 0xFC60;
static const int RADIAL_POT_R = 12;

RemoteGraphicMode::RemoteGraphicMode(TFTHandler& tftRef, Hardware& hwRef)
  : Mode(tftRef), hw(hwRef) {}

void RemoteGraphicMode::enter() {
  // Just clear once; the canvas handles the rest in update
  tft.clearScreen(); 
}

void RemoteGraphicMode::update() {
  // 1. Reference the Global Canvas
  TFT_eSprite& c = tft.canvas;
  
  // 2. Clear the canvas (RAM) for the new frame
  c.fillSprite(TFT_BLACK);

  // 3. Draw Title
  tft.drawCenteredText("Remote Control Graphic", 8, TFT_CYAN, 1);

  // 4. Draw Outlines (Self-healing is automatic now)
  c.drawRect(46, 61, 69, 30, TFT_CYAN); // IMU Rect
  c.drawRect(46, 94, 69, 33, TFT_RED);  // Keyboard Rect

  // 5. Draw D-Pad Buttons
  int btnX[] = {56, 82, 69, 69, 105}; 
  int btnY[] = {110, 110, 102, 118, 110};
  const char* bNames[] = {"LEFT", "RIGHT", "UP", "DOWN", "SELECT"};
  String key = hw.keyboard.getCurrentKey(); 

  for(int i=0; i<5; i++) {
    c.drawCircle(btnX[i], btnY[i], 6, TFT_YELLOW); // Outline
    if (key == bNames[i]) {
        c.fillCircle(btnX[i], btnY[i], 4, TFT_RED); // Press highlight
    }
  }

  // 6. Radial Dials (Simplified: No more manual erasing)
  auto drawRadial = [&](int cx, int cy, int r, float v, uint16_t color) {
    c.drawCircle(cx, cy, r, TFT_WHITE);
    float start = -M_PI_2;
    float end = start + (v * M_PI);
    for (float a = (v > 0 ? start : end); a <= (v > 0 ? end : start); a += 0.2) {
      c.drawLine(cx, cy, cx + cos(a) * r, cy + sin(a) * r, color);
    }
    char buf[8]; snprintf(buf, sizeof(buf), "%+4.1f", v);
    c.setTextColor(color);
    c.drawCentreString(buf, cx, cy + r + 4, 1);
  };

  drawRadial(40, 35, RADIAL_POT_R, hw.state.potL, TFT_RED);
  drawRadial(80, 35, RADIAL_POT_R, hw.state.potM, TFT_GREEN);
  drawRadial(120, 35, RADIAL_POT_R, hw.state.potR, TFT_BLUE);

  // 7. Joysticks
  const int left_cx = 23, right_cx = 136, j_cy = 110, j_r = 15;
  c.drawCircle(left_cx, j_cy, j_r, TFT_WHITE);
  c.drawCircle(right_cx, j_cy, j_r, TFT_WHITE);

  c.fillCircle(left_cx + (hw.state.joyLX * (j_r - 6)), j_cy - (hw.state.joyLY * (j_r - 6)), JOYSTICK_DOT, JOYSTICK_COLOR);
  c.fillCircle(right_cx + (hw.state.joyRX * (j_r - 6)), j_cy - (hw.state.joyRY * (j_r - 6)), JOYSTICK_DOT, JOYSTICK_COLOR);

// 8. Encoders (3-pixel thick outline + Click Highlight)
  auto drawEnc = [&](int cx, int cy, long val, bool isPressed, uint16_t color) {
      // 1. Draw 3-pixel thick outline
      c.drawCircle(cx, cy, 13, color);
      c.drawCircle(cx, cy, 12, color);
      c.drawCircle(cx, cy, 11, color);
      
      // 2. [NEW] If pressed, fill the center to highlight
      if (isPressed) {
          c.fillCircle(cx, cy, 10, TFT_RED); 
          c.setTextColor(TFT_BLACK); // Invert text color so it's readable
      } else {
          c.setTextColor(color);
      }
      
      char eb[8]; 
      snprintf(eb, sizeof(eb), "%ld", val);
      c.drawCentreString(eb, cx, cy - 4, 1);
  };

  // Assuming Encoder Left is bit 5 and Encoder Right is bit 6 in your button mask
  // Adjust the (1 << bit) if your hardware mapping is different
  bool leftPressed  = (hw.state.buttons & (1 << 5)); 
  bool rightPressed = (hw.state.buttons & (1 << 6));

  drawEnc(23, 72, hw.state.encL, leftPressed, ENCODER_COLOR);
  drawEnc(136, 72, hw.state.encR, rightPressed, ENCODER_COLOR);

  // 9. IMU Ball
  const int imu_cx = 80, imu_cy = 76, imu_r = 10;
  int ballX = imu_cx + (int)(-(constrain(hw.state.ax, -4.0, 4.0)/4.0) * (imu_r - 2));
  int ballY = imu_cy + (int)(-(constrain(hw.state.ay, -2.0, 2.0)/2.0) * (imu_r - 2));
  
  c.drawCircle(imu_cx, imu_cy, imu_r, TFT_WHITE);
  c.fillCircle(ballX, ballY, 3, TFT_YELLOW);

  // 10. [CRITICAL] Push the finished canvas to the display
  tft.updateDisplay();
}

void RemoteGraphicMode::exit() {}