#include "RemoteControlMode.h"
#include <Arduino.h>

extern bool Switch1On;
extern bool Switch2On;

RemoteControlMode::RemoteControlMode(TFTHandler& tftRef, Hardware& hw)
    : Mode(tftRef),
      joyRight(hw.joyRight), joyLeft(hw.joyLeft), keyboard(hw.keyboard),
      encoderRight(hw.encoderRight), encoderLeft(hw.encoderLeft),
      potRight(hw.potRight), potMid(hw.potMid), potLeft(hw.potLeft), imu(hw.imu)
{
  Serial.println("RemoteControlMode constructed - All sensors ready (raw joystick printing enabled)");
}

void RemoteControlMode::enter() {
  tft.clearScreen();
  tft.drawCenteredText("Remote Control", 8, TFT_CYAN, 1);

  delay(500);  // Extra safety

  if (!imu.begin()) {
    Serial.println("IMU init failed - check wiring!");
  } else {
    Serial.println("IMU initialized successfully");
  }
}

void RemoteControlMode::update() {

  // === Read all current values ===
  String currentKey = keyboard.getCurrentKey();
  String pressedKey = keyboard.getPressedKey();

  // Update joysticks (applies deadzone and mapping)
  joyRight.update();
  joyLeft.update();

  // === RAW VALUES FOR CALIBRATION ===
  int rawRX = joyRight.getRawX();
  int rawRY = joyRight.getRawY();
  int rawLX = joyLeft.getRawX();
  int rawLY = joyLeft.getRawY();

  // Mapped values for display
  float joyRX = joyRight.getMappedX();
  float joyRY = joyRight.getMappedY();
  float joyLX = joyLeft.getMappedX();
  float joyLY = joyLeft.getMappedY();

  long encRight = encoderRight.getCounter();
  long encLeft  = encoderLeft.getCounter();
  bool encRBtn  = encoderRight.isButtonPressed();
  bool encLBtn  = encoderLeft.isButtonPressed();

  // === Potentiometers ===
  potLeft.update();
  potMid.update();
  potRight.update();

  int potL_raw = potLeft.getRaw();
  int potM_raw = potMid.getRaw();
  int potR_raw = potRight.getRaw();

  // Normalized values for display (with deadzone)
  float potL_norm = potLeft.getNormalized();
  float potM_norm = potMid.getNormalized();
  float potR_norm = potRight.getNormalized();

  // === Safe IMU read at 10Hz ===
  static bool imuReady = false;
  if (!imuReady) {
    if (imu.begin()) {
      imuReady = true;
    } else {
      return;  // Try again next loop
    }
  }
  static unsigned long lastIMURead = 0;
  if (millis() - lastIMURead >= 100 && imuReady) {
    imu.read();
    lastIMURead = millis();
  }

  // === Flicker-free redraw only on change ===
  static String lastKey = "";
  static float lastRX = -10.0f, lastRY = -10.0f, lastLX = -10.0f, lastLY = -10.0f;
  static long lastEncR = -999, lastEncL = -999;
  static bool lastRBtn = false, lastLBtn = false;
  static float lastPotL_norm = -10.0f, lastPotM_norm = -10.0f, lastPotR_norm = -10.0f;
  static float lastAX = -999, lastAY = -999, lastAZ = -999;
  static float lastGX = -999, lastGY = -999, lastGZ = -999;

  bool changed = (currentKey != lastKey) ||
                 (abs(joyRX - lastRX) > 0.05f) || (abs(joyRY - lastRY) > 0.05f) ||
                 (abs(joyLX - lastLX) > 0.05f) || (abs(joyLY - lastLY) > 0.05f) ||
                 (encRight != lastEncR) || (encLeft != lastEncL) ||
                 (encRBtn != lastRBtn) || (encLBtn != lastLBtn) ||
                 (abs(potL_norm - lastPotL_norm) > 0.05f) ||
                 (abs(potM_norm - lastPotM_norm) > 0.05f) ||
                 (abs(potR_norm - lastPotR_norm) > 0.05f) ||
                 (abs(imu.ax - lastAX) > 0.1) || (abs(imu.ay - lastAY) > 0.1) || (abs(imu.az - lastAZ) > 0.1) ||
                 (abs(imu.gx - lastGX) > 1.0) || (abs(imu.gy - lastGY) > 1.0) || (abs(imu.gz - lastGZ) > 1.0);

  if (!changed) {
    delay(50);
    return;
  }

  // === Print to Serial Monitor whenever there is a change ===
  /*
  Serial.printf("Keyboard:\t%s\n", pressedKey="NONE" ? "None":pressedKey.c_str());
  Serial.printf("RAW JS_R:\tX=%4d\tY=%4d\t\t JS_R:\tX=%4d\tY=%4d\n", rawLX, rawLY, rawRX, rawRY);
  Serial.printf("Enc:\tL=%4d %s\tR=%5ld %s\n",
                encLeft,  encLBtn ? "Select" : "None",
                encRight, encRBtn ? "Select" : "None");
  Serial.printf("POT RAW:\tL=%4d\tM=%4d\tR=%4d\n", potL_raw, potM_raw, potR_raw);
  Serial.printf("IMU Accl X:%.1f Y:%.1f Z:%.1f  Gyro X:%.1f Y:%.1f Z:%.1f\n",
                imu.ax, imu.ay, imu.az, imu.gx, imu.gy, imu.gz);
  Serial.printf("\n");
  */
  Serial.printf("Key:%s JL X%4d Y%4d JR X%4d Y%4d EncL%2d %s EncR%2d POT:L%4d M%4d R%4d IMU AcclX:%+4.1f Y:%+4.1f Z:%+4.1f  Gyro X:%+4.1f Y:%+4.1f Z:%+4.1f\n",
                pressedKey == "NONE" ? "None" : pressedKey.c_str(),
                rawLX, rawLY,
                rawRX, rawRY,
                encLeft,  encLBtn ? "Select" : "None",
                encRight, 
                potL_raw, potM_raw, potR_raw,
                imu.ax, imu.ay, imu.az,
                imu.gx, imu.gy, imu.gz);

  // Update last values
  lastKey = currentKey;
  lastRX = joyRX; lastRY = joyRY;
  lastLX = joyLX; lastLY = joyLY;
  lastEncR = encRight; lastEncL = encLeft;
  lastRBtn = encRBtn; lastLBtn = encLBtn;
  lastPotL_norm = potL_norm; lastPotM_norm = potM_norm; lastPotR_norm = potR_norm;
  lastAX = imu.ax; lastAY = imu.ay; lastAZ = imu.az;
  lastGX = imu.gx; lastGY = imu.gy; lastGZ = imu.gz;

  // === Display settings ===
  const int textSize = 1;
  const int baseCharHeight = 8;
  const int charHeight = baseCharHeight * textSize;
  const int lineSpacing = 4;
  const int startX = 10;
  const int startY = 24;

  int lineNum = 0;

  tft.getTFT().setTextSize(textSize);

  auto printLine = [&](uint16_t color, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[80];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    int y = startY + (lineNum++ * (charHeight + lineSpacing));
    int lineTop = y - (charHeight - 4); // Approximate top of text (adjust -2 if needed)
    tft.getTFT().fillRect(startX, lineTop, 154, charHeight + 3, TFT_BLACK);
    //tft.getTFT().fillRect(startX, y - charHeight/2 + 2, 140, charHeight + 2, TFT_BLACK);
    tft.getTFT().setTextColor(color);
    tft.getTFT().setCursor(startX, y);
    tft.getTFT().println(buffer);
  };

  // === Lines ===
  printLine(TFT_LIGHTGREY, "Switch 1:%s Switch 2:%s", Switch1On ? "On " : "Off", Switch2On ? "On " : "Off");

  if (currentKey != "NONE") {
    printLine(TFT_RED, "Keyboard: %s", currentKey.c_str());
  } else {
    printLine(TFT_YELLOW, "Keyboard: None");
  }

  printLine(TFT_SKYBLUE, "JS_R: %+5.2f %+5.2f", joyRX, joyRY);
  printLine(TFT_SKYBLUE, "JS_L: %+5.2f %+5.2f", joyLX, joyLY);

  printLine(TFT_ORANGE, "EncR: %3ld  %s", encRight, encRBtn ? "Select" : "None");
  printLine(TFT_ORANGE, "EncL: %3ld  %s", encLeft,  encLBtn ? "Select" : "None");

  // Pots - normalized -1.00 to 1.00 (with deadzone for stable display)
  printLine(TFT_GREEN, "PotL:%+4.1f M:%+4.1f R:%+4.1f", potL_norm, potM_norm, potR_norm);

  // IMU lines
  printLine(TFT_MAGENTA, "AcclX:%+4.1f Y:%+4.1f Z:%+4.1f", imu.ax, imu.ay, imu.az);
  printLine(TFT_MAGENTA, "GyroX:%+4.1f Y:%+4.1f Z:%+4.1f", imu.gx, imu.gy, imu.gz);

  delay(50);
}

void RemoteControlMode::exit() {
  Serial.println("Leaving Remote Control Mode");
}