#include "system/GraphicView.h"
#include <Arduino.h>
#include <math.h>

static const int JOYSTICK_DOT = 3;
static const uint16_t JOYSTICK_COLOR = TFT_RED;
static const uint16_t ENCODER_COLOR = 0xFC60;
static const int RADIAL_POT_R = 12;

GraphicView::GraphicView(TFTHandler& tftRef, Hardware& hwRef)
  : Activity(tftRef, hwRef) {} // FIXED: Pass both to Activity base class

void GraphicView::enter() {
  tft.clearScreen(); 
}

void GraphicView::update() {
  // 1. Reference the Global Canvas (Inherited)
  TFT_eSprite& c = tft.canvas;
  
  // 2. Clear the canvas RAM for the new frame
  c.fillSprite(TFT_BLACK);

  // 3. Draw Title
  tft.drawCenteredText("GRAPHIC VIEW", 8, TFT_CYAN, 1);

  // 4. Draw Outlines
  c.drawRect(46, 61, 69, 30, TFT_CYAN); // IMU Rect
  c.drawRect(46, 94, 69, 33, TFT_RED);  // Keyboard Rect

  // 5. Draw D-Pad Buttons
  int btnX[] = {56, 82, 69, 69, 105}; 
  int btnY[] = {110, 110, 102, 118, 110};
  const char* bNames[] = {"LEFT", "RIGHT", "UP", "DOWN", "SELECT"};
  String key = hw.keyboard.getCurrentKey(); // Using inherited 'hw'

  for(int i=0; i<5; i++) {
    c.drawCircle(btnX[i], btnY[i], 6, TFT_YELLOW); 
    if (key == bNames[i]) {
        c.fillCircle(btnX[i], btnY[i], 4, TFT_RED); 
    }
  }

  // 6. Radial Dials
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
  
  c.drawCircle(right_cx, j_cy, j_r, TFT_WHITE);
  c.fillCircle(right_cx + (hw.state.joyRX * (j_r - 6)), j_cy - (hw.state.joyRY * (j_r - 6)), JOYSTICK_DOT, JOYSTICK_COLOR);
  c.fillCircle(left_cx + (hw.state.joyLX * (j_r - 6)), j_cy - (hw.state.joyLY * (j_r - 6)), JOYSTICK_DOT, JOYSTICK_COLOR);
  

  if (WiFi.getMode() == WIFI_OFF){ //Gray out when WiFi is on
    c.drawCircle(left_cx, j_cy, j_r, TFT_WHITE);
    c.fillCircle(left_cx + (hw.state.joyLX * (j_r - 6)), j_cy - (hw.state.joyLY * (j_r - 6)), JOYSTICK_DOT, JOYSTICK_COLOR);
  }else{
    c.drawCircle(left_cx, j_cy, j_r, TFT_DARKGREY);
    c.fillCircle(left_cx + (hw.state.joyLX * (j_r - 6)), j_cy - (hw.state.joyLY * (j_r - 6)), JOYSTICK_DOT, TFT_DARKGREY);
  }
  
  //  ---
  // 8. Encoders
  auto drawEnc = [&](int cx, int cy, long val, bool isPressed, uint16_t color) {
      c.drawCircle(cx, cy, 13, color);
      c.drawCircle(cx, cy, 12, color);
      c.drawCircle(cx, cy, 11, color);
      
      if (isPressed) {
          c.fillCircle(cx, cy, 10, TFT_RED); 
          c.setTextColor(TFT_BLACK); 
      } else {
          c.setTextColor(color);
      }
      
      char eb[8]; 
      snprintf(eb, sizeof(eb), "%ld", val);
      c.drawCentreString(eb, cx, cy - 4, 1);
  };

  bool leftPressed  = (hw.state.buttons & (1 << 5)); 
  bool rightPressed = (hw.state.buttons & (1 << 6));

  drawEnc(23, 72, hw.state.encL, leftPressed, ENCODER_COLOR);
  drawEnc(136, 72, hw.state.encR, rightPressed, ENCODER_COLOR);

  // 9. IMU Ball (Tilt Visualization)
  const int imu_cx = 80, imu_cy = 76, imu_r = 10;
  int ballX = imu_cx + (int)(-(constrain(hw.state.ax, -4.0, 4.0)/4.0) * (imu_r - 2));
  int ballY = imu_cy + (int)(-(constrain(hw.state.ay, -2.0, 2.0)/2.0) * (imu_r - 2));
  
  c.drawCircle(imu_cx, imu_cy, imu_r, TFT_WHITE);
  c.fillCircle(ballX, ballY, 3, TFT_YELLOW);

  // 10. Push the finished canvas to the display
  tft.updateDisplay();
}

void GraphicView::exit() {}