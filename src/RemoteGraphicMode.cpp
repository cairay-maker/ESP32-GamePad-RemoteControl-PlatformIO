#include "RemoteGraphicMode.h"
#include <Arduino.h>
#include <math.h>

static const int JOYSTICK_DOT = 3; // smaller dot for both joysticks
static const uint16_t JOYSTICK_COLOR = TFT_RED; // use red for joystick dots
static const uint16_t ENCODER_COLOR = 0xFC60; // reddish-orange (RGB565)
static const int RADIAL_POT_R = 12; // outer radius for radial encoders
static const int RADIAL_STEPS = 16; // map ±16 counts -> ±1.0

RemoteGraphicMode::RemoteGraphicMode(TFTHandler& tftRef, Hardware& hw)
  : Mode(tftRef),
    joyRight(hw.joyRight), joyLeft(hw.joyLeft), keyboard(hw.keyboard),
    encoderRight(hw.encoderRight), encoderLeft(hw.encoderLeft), encoderMiddle(hw.encoderMiddle),
    potLeft(hw.potLeft), potMid(hw.potMid), potRight(hw.potRight), imu(hw.imu)
{
}

void RemoteGraphicMode::enter() {
  tft.clearScreen();
  tft.drawCenteredText("Remote Control Graphic", 8, TFT_CYAN, 1);
  // Draw static circles for left and right joysticks at fixed coordinates
  TFT_eSPI& disp = tft.getTFT();
  const int left_cx = 23;
  const int right_cx = 136; // fixed right position per request
  const int cy = 110;
  const int radius = 15;

  disp.drawCircle(left_cx, cy, radius, TFT_WHITE);
    disp.fillCircle(left_cx, cy, JOYSTICK_DOT, JOYSTICK_COLOR);

  disp.drawCircle(right_cx, cy, radius, TFT_WHITE);
    disp.fillCircle(right_cx, cy, JOYSTICK_DOT, JOYSTICK_COLOR);

  // Draw rectangle frame (upper-left corner x=46, y=88)
  const int rect_x = 46;
  const int rect_y = 94;
  const int rect_w = 69;
  const int rect_h = 33;
    disp.drawRect(rect_x, rect_y, rect_w, rect_h, TFT_RED);

  // Draw IMU reserved area (upper box) at x=46, y=38, w=69, h=40
  const int imu_x = 46;
    const int imu_y = 61;
    const int imu_w = 69;
    const int imu_h = 30;
  disp.drawRect(imu_x, imu_y, imu_w, imu_h, TFT_CYAN);
  // Reserve inner area (fill with black) for future IMU graphs
  disp.fillRect(imu_x + 1, imu_y + 12, imu_w - 2, imu_h - 13, TFT_BLACK);
  // Draw a fixed outer circle (play area) and initial ball centered inside the IMU box
  const int imu_cx = imu_x + imu_w / 2;
  const int imu_cy = imu_y + imu_h / 2;
  const int ball_r = 4;
  const int fixed_r = 10; // slightly bigger than ball
  // clear inner IMU area already done above; draw fixed circle outline
  disp.drawCircle(imu_cx, imu_cy, fixed_r, TFT_WHITE);
  // draw initial ball at center
  disp.fillCircle(imu_cx, imu_cy, ball_r, TFT_YELLOW);

  // Draw circular buttons (outline rings) with provided coordinates
  // Button coords and radius per request
  const int btn_left_x = 56;
  const int btn_left_y = 110;
    const int btn_right_x = 82;
  const int btn_right_y = 110;
  const int btn_up_x = 69;
  const int btn_up_y = 102;
  const int btn_down_x = 69;
  const int btn_down_y = 118;
  const int btn_select_x = 105;
  const int btn_select_y = 110;
  const int btn_r = 6; // requested radius
  const int inset = 2;

  // Draw rings: outer yellow, inner black to create visible yellow outline
  disp.fillCircle(btn_left_x, btn_left_y, btn_r, TFT_YELLOW);
  disp.fillCircle(btn_left_x, btn_left_y, btn_r - inset, TFT_BLACK);

  disp.fillCircle(btn_right_x, btn_right_y, btn_r, TFT_YELLOW);
  disp.fillCircle(btn_right_x, btn_right_y, btn_r - inset, TFT_BLACK);

  disp.fillCircle(btn_up_x, btn_up_y, btn_r, TFT_YELLOW);
  disp.fillCircle(btn_up_x, btn_up_y, btn_r - inset, TFT_BLACK);

  disp.fillCircle(btn_down_x, btn_down_y, btn_r, TFT_YELLOW);
  disp.fillCircle(btn_down_x, btn_down_y, btn_r - inset, TFT_BLACK);

  disp.fillCircle(btn_select_x, btn_select_y, btn_r, TFT_YELLOW);
  disp.fillCircle(btn_select_x, btn_select_y, btn_r - inset, TFT_BLACK);

  // Draw encoder rings and initial values (previous rotary encoders)
  const int encL_cx = 23;
  const int encL_cy = 72;
  const int encR_cx = 136;
  const int encR_cy = 72; // updated per request
  const int enc_outer_r = 13;
  const int enc_inner_r = 10;

  // Left encoder ring (reddish-orange)
  disp.fillCircle(encL_cx, encL_cy, enc_outer_r, ENCODER_COLOR);
  disp.fillCircle(encL_cx, encL_cy, enc_inner_r, TFT_BLACK);
  // Right encoder ring (reddish-orange)
  disp.fillCircle(encR_cx, encR_cy, enc_outer_r, ENCODER_COLOR);
  disp.fillCircle(encR_cx, encR_cy, enc_inner_r, TFT_BLACK);

  // Initial encoder text
  char buf[16];
  snprintf(buf, sizeof(buf), "%+2ld", encoderLeft.getCounter());
  disp.setTextSize(1);
  disp.setTextColor(ENCODER_COLOR);
  int16_t tw = disp.textWidth(buf);
  disp.setCursor(encL_cx - tw/2, encL_cy - 4);
  disp.println(buf);

  snprintf(buf, sizeof(buf), "%+2ld", encoderRight.getCounter());
  disp.setTextColor(ENCODER_COLOR);
  tw = disp.textWidth(buf);
  disp.setCursor(encR_cx - tw/2, encR_cy - 4);
  disp.println(buf);

  // Draw three radial encoders (Left, Middle, Right) near top area
  const int rPotL_x = 40;
  const int rPotM_x = 80;
  const int rPotR_x = 120;
  const int rPot_y = 35;
  // outer circle outlines (use white outline, inner cleared)
  disp.drawCircle(rPotL_x, rPot_y, RADIAL_POT_R, TFT_WHITE);
  disp.fillCircle(rPotL_x, rPot_y, RADIAL_POT_R - 3, TFT_BLACK);
  disp.drawCircle(rPotM_x, rPot_y, RADIAL_POT_R, TFT_WHITE);
  disp.fillCircle(rPotM_x, rPot_y, RADIAL_POT_R - 3, TFT_BLACK);
  disp.drawCircle(rPotR_x, rPot_y, RADIAL_POT_R, TFT_WHITE);
  disp.fillCircle(rPotR_x, rPot_y, RADIAL_POT_R - 3, TFT_BLACK);
  // initial radial wipes (center line) in per-dial colors
  disp.drawLine(rPotL_x, rPot_y, rPotL_x, rPot_y - RADIAL_POT_R, TFT_RED);
  disp.drawLine(rPotM_x, rPot_y, rPotM_x, rPot_y - RADIAL_POT_R, TFT_GREEN);
  disp.drawLine(rPotR_x, rPot_y, rPotR_x, rPot_y - RADIAL_POT_R, TFT_BLUE);
}

void RemoteGraphicMode::update() {
  // Update both joysticks
  joyLeft.update();
  joyRight.update();

  // Read current analog keyboard state (UP/DOWN/LEFT/RIGHT/SELECT/NONE)
  String key = keyboard.getCurrentKey();

  float lmx = joyLeft.getMappedX();
  float lmy = joyLeft.getMappedY();
  float rmx = joyRight.getMappedX();
  float rmy = joyRight.getMappedY();

  TFT_eSPI& disp = tft.getTFT();
  const int left_cx = 23;
  const int right_cx = 136; // fixed per request
  const int cy = 110;
  const int radius = 15;

  int lx = left_cx + static_cast<int>(lmx * (radius - 6));
  int ly = cy - static_cast<int>(lmy * (radius - 6));

  int rx = right_cx + static_cast<int>(rmx * (radius - 6));
  int ry = cy - static_cast<int>(rmy * (radius - 6));

  // Clamp to display bounds to avoid drawing off-screen
  lx = constrain(lx, 0, disp.width() - 1);
  ly = constrain(ly, 0, disp.height() - 1);
  rx = constrain(rx, 0, disp.width() - 1);
  ry = constrain(ry, 0, disp.height() - 1);

  static bool first = true;
  static int lastLX = left_cx, lastLY = cy;
  static int lastRX = right_cx, lastRY = cy;

  if (first) {
    lastLX = left_cx; lastLY = cy;
    lastRX = right_cx; lastRY = cy;
    first = false;
  }

  // Update left dot
  if (lastLX != lx || lastLY != ly) {
    disp.fillCircle(lastLX, lastLY, JOYSTICK_DOT, TFT_BLACK);
    disp.drawCircle(left_cx, cy, radius, TFT_WHITE);
      disp.fillCircle(lx, ly, JOYSTICK_DOT, JOYSTICK_COLOR);
    lastLX = lx; lastLY = ly;
  }

  // Update right dot
  if (lastRX != rx || lastRY != ry) {
    disp.fillCircle(lastRX, lastRY, JOYSTICK_DOT, TFT_BLACK);
    disp.drawCircle(right_cx, cy, radius, TFT_WHITE);
      disp.fillCircle(rx, ry, JOYSTICK_DOT, JOYSTICK_COLOR);
    lastRX = rx; lastRY = ry;
  }

  // Update encoder displays and right-button fill toggle
  const int encL_cx = 23;
  const int encL_cy = 72;
  const int encR_cx = 136;
  const int encR_cy = 72;
  const int enc_outer_r = 13;
  const int enc_inner_r = 10;

  static long lastEncL = LONG_MIN;
  static long lastEncR = LONG_MIN;

  long curEncL = encoderLeft.getCounter();
  long curEncR = encoderRight.getCounter();

  // update potentiometers for radial displays
  potLeft.update();
  potMid.update();
  potRight.update();
  float potL_norm = potLeft.getNormalized();
  float potM_norm = potMid.getNormalized();
  float potR_norm = potRight.getNormalized();

  // --- IMU-driven rolling ball ---
  // IMU box geometry (must match enter())
  const int imu_x = 46;
  const int imu_y = 61;
  const int imu_w = 69;
  const int imu_h = 30;
  const int imu_cx = imu_x + imu_w / 2;
  const int imu_cy = imu_y + imu_h / 2;
  const int ball_r = 4;
  const int fixed_r = 10;

  // Read IMU and map to constrained values
  imu.update();
  float ax = imu.ax; // positive -> roll left per request
  float ay = imu.ay; // positive -> roll up per request
  // constrain to requested ranges
  ax = constrain(ax, -4.0f, 4.0f);
  ay = constrain(ay, -2.0f, 2.0f);
  // normalize to -1..1
  float nx = ax / 4.0f;
  float ny = ay / 2.0f;

  // Map normalized values to pixel offsets inside the IMU rectangle
  // available travel is rectangle half-size minus ball radius and a 1px margin
  int availX = (imu_w / 2) - ball_r - 1;
  int availY = (imu_h / 2) - ball_r - 1;
  // positive ax -> ball moves left (negative x offset)
  int offx = static_cast<int>(-nx * availX);
  int offy = static_cast<int>(-ny * availY);

  int ballX = imu_cx + offx;
  int ballY = imu_cy + offy;

  static int lastBallX = imu_cx;
  static int lastBallY = imu_cy;
  // erase last ball and redraw fixed circle to avoid artifacts
  if (lastBallX != ballX || lastBallY != ballY) {
    // Erase previous ball using inner IMU background color, then restore IMU box outline
    disp.fillCircle(lastBallX, lastBallY, ball_r + 1, TFT_BLACK);
    // Redraw inner IMU fill area and outline to avoid erasing borders
    disp.fillRect(imu_x + 1, imu_y + 12, imu_w - 2, imu_h - 13, TFT_BLACK);
    disp.drawRect(imu_x, imu_y, imu_w, imu_h, TFT_CYAN);
    // Redraw fixed circle on top of the restored background
    disp.drawCircle(imu_cx, imu_cy, fixed_r, TFT_WHITE);
    lastBallX = ballX; lastBallY = ballY;
  }
  // draw new ball
  disp.fillCircle(ballX, ballY, ball_r, TFT_YELLOW);

  if (curEncL != lastEncL) {
    // redraw left ring (reddish-orange outline)
    disp.fillCircle(encL_cx, encL_cy, enc_outer_r, ENCODER_COLOR);
    disp.fillCircle(encL_cx, encL_cy, enc_inner_r, TFT_BLACK);
    // draw value
    char buf[16];
    snprintf(buf, sizeof(buf), "%+2ld", curEncL);
    disp.setTextSize(1);
    disp.setTextColor(ENCODER_COLOR);
    int16_t tw = disp.textWidth(buf);
    disp.setCursor(encL_cx - tw/2, encL_cy - 4);
    disp.println(buf);
    lastEncL = curEncL;
  }

  // Radial encoders: Left, Middle, Right at y=30 with outer radius RADIAL_POT_R
  const int rPotL_x = 40;
  const int rPotM_x = 80;
  const int rPotR_x = 120;
  const int rPot_y = 35;

  auto drawRadial = [&](int cx, int cy, int r, float v, uint16_t textColor) {
    // v is normalized in [-1,1]
    if (v > 1.0f) v = 1.0f; if (v < -1.0f) v = -1.0f;
    // clear inner area and redraw outer ring (white outline)
    disp.fillCircle(cx, cy, r - 3, TFT_BLACK);
    disp.drawCircle(cx, cy, r, TFT_WHITE);

    const float start = -M_PI_2; // top
    const float sweep = 180.0f * (M_PI / 180.0f); // ±180 degrees full range
    float end = start + v * sweep;

    // If v == 0: draw single vertical center line in dial color
    if (fabs(v) < 1e-6) {
      disp.drawLine(cx, cy, cx, cy - r, textColor);
    } else {
      // draw radial wipe from start to end (step ~3 degrees)
      const float step = 3.0f * (M_PI / 180.0f);
      if (end > start) {
        for (float a = start; a <= end; a += step) {
          int x = cx + static_cast<int>(cos(a) * r);
          int y = cy + static_cast<int>(sin(a) * r);
          disp.drawLine(cx, cy, x, y, textColor);
        }
      } else {
        for (float a = start; a >= end; a -= step) {
          int x = cx + static_cast<int>(cos(a) * r);
          int y = cy + static_cast<int>(sin(a) * r);
          disp.drawLine(cx, cy, x, y, textColor);
        }
      }
    }

    // Draw the numeric value centered below the circle
    char valbuf[16];
    snprintf(valbuf, sizeof(valbuf), "%+5.2f", v);
    disp.setTextSize(1);
    int16_t tw = disp.textWidth(valbuf);
    int textX = cx - tw / 2;
    int textY = cy + r + 2;
    // clear previous text area
    disp.fillRect(textX - 1, textY - 1, tw + 2, 10, TFT_BLACK);
    disp.setTextColor(textColor);
    disp.setCursor(textX, textY);
    disp.println(valbuf);
  };

  // Only redraw radial encoders when changed
  static float lastRadL = 9999.0f, lastRadM = 9999.0f, lastRadR = 9999.0f;
  if (fabs(potL_norm - lastRadL) > 0.001f) { drawRadial(rPotL_x, rPot_y, RADIAL_POT_R, potL_norm, TFT_RED); lastRadL = potL_norm; }
  if (fabs(potM_norm - lastRadM) > 0.001f) { drawRadial(rPotM_x, rPot_y, RADIAL_POT_R, potM_norm, TFT_GREEN); lastRadM = potM_norm; }
  if (fabs(potR_norm - lastRadR) > 0.001f) { drawRadial(rPotR_x, rPot_y, RADIAL_POT_R, potR_norm, TFT_BLUE); lastRadR = potR_norm; }

  // Right encoder highlight: blink once on press (redraw on press AND release)
  static bool lastRightPressed = false;
  bool rightPressed = encoderRight.isButtonPressed();

  if (curEncR != lastEncR || rightPressed != lastRightPressed) {
    // redraw right ring; if currently pressed fill inner with YELLOW, otherwise black
    disp.fillCircle(encR_cx, encR_cy, enc_outer_r, ENCODER_COLOR);
    if (rightPressed) {
      disp.fillCircle(encR_cx, encR_cy, enc_inner_r, TFT_YELLOW);
    } else {
      disp.fillCircle(encR_cx, encR_cy, enc_inner_r, TFT_BLACK);
    }
    // draw encoder value
    char buf2[16];
    snprintf(buf2, sizeof(buf2), "%+2ld", curEncR);
    disp.setTextSize(1);
    disp.setTextColor(ENCODER_COLOR);
    int16_t tw2 = disp.textWidth(buf2);
    disp.setCursor(encR_cx - tw2/2, encR_cy - 4);
    disp.println(buf2);
    lastEncR = curEncR;
    lastRightPressed = rightPressed;
  }

  // Update D-pad/select visuals: draw yellow outline rings then fill active key
  const int btn_left_x = 56;
  const int btn_left_y = 110;
    const int btn_right_x = 82;
  const int btn_right_y = 110;
  const int btn_up_x = 69;
  const int btn_up_y = 102;
  const int btn_down_x = 69;
  const int btn_down_y = 118;
  const int btn_select_x = 105;
  const int btn_select_y = 110;
  const int btn_r = 6;
  const int inset = 2;

  // Draw rings (outline)
  disp.fillCircle(btn_left_x, btn_left_y, btn_r, TFT_YELLOW);
  disp.fillCircle(btn_left_x, btn_left_y, btn_r - inset, TFT_BLACK);

  disp.fillCircle(btn_right_x, btn_right_y, btn_r, TFT_YELLOW);
  disp.fillCircle(btn_right_x, btn_right_y, btn_r - inset, TFT_BLACK);

  disp.fillCircle(btn_up_x, btn_up_y, btn_r, TFT_YELLOW);
  disp.fillCircle(btn_up_x, btn_up_y, btn_r - inset, TFT_BLACK);

  disp.fillCircle(btn_down_x, btn_down_y, btn_r, TFT_YELLOW);
  disp.fillCircle(btn_down_x, btn_down_y, btn_r - inset, TFT_BLACK);

  disp.fillCircle(btn_select_x, btn_select_y, btn_r, TFT_YELLOW);
  disp.fillCircle(btn_select_x, btn_select_y, btn_r - inset, TFT_BLACK);

  // Fill active key with solid yellow
  if (key == "UP") {
    disp.fillCircle(btn_up_x, btn_up_y, btn_r, TFT_YELLOW);
  } else if (key == "DOWN") {
    disp.fillCircle(btn_down_x, btn_down_y, btn_r, TFT_YELLOW);
  } else if (key == "LEFT") {
    disp.fillCircle(btn_left_x, btn_left_y, btn_r, TFT_YELLOW);
  } else if (key == "RIGHT") {
    disp.fillCircle(btn_right_x, btn_right_y, btn_r, TFT_YELLOW);
  } else if (key == "SELECT") {
    disp.fillCircle(btn_select_x, btn_select_y, btn_r, TFT_YELLOW);
  }

  delay(30);
}

void RemoteGraphicMode::exit() {
  // nothing special
}
