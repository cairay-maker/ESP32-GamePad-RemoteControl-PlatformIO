#include "system/DataView.h"
#include <Arduino.h>

DataView::DataView(TFTHandler& tftRef, Hardware& hwRef)
    : Activity(tftRef, hwRef) // FIXED: Pass both references to Activity
{
  Serial.println("DataView constructed - Using Global Canvas");
}

void DataView::enter() {
  tft.clearScreen();
}

void DataView::update() {
  // 1. Reference the Global Canvas (inherited from Activity)
  TFT_eSprite& c = tft.canvas;

  // 2. Clear the canvas for the new frame
  c.fillSprite(TFT_BLACK);

  // 3. Draw Header
  tft.drawCenteredText("Remote Control Data", 8, TFT_CYAN, 1);

  // 4. Data Extraction from unified hardware state (inherited 'hw')
  bool s1 = (hw.state.buttons & (1 << 7));
  bool s2 = (hw.state.buttons & (1 << 8));
  String currentKey = hw.keyboard.getCurrentKey(); 
  bool encRBtn  = (hw.state.buttons & (1 << 6));
  bool encLBtn  = (hw.state.buttons & (1 << 5));

  // 5. Layout Constants
  const int startX = 10;
  const int startY = 24;
  const int lineSpacing = 11; 
  int lineNum = 0;

  auto printLine = [&](uint16_t color, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[80];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    int y = startY + (lineNum++ * lineSpacing);
    c.setTextColor(color);
    c.setCursor(startX, y);
    c.print(buffer);
  };

  // 6. Draw the Data Lines
  printLine(TFT_LIGHTGREY, "Switch 1:%s Switch 2:%s", s1 ? "On " : "Off", s2 ? "On " : "Off");

  if (currentKey != "NONE") {
    printLine(TFT_RED, "Keyboard: %s", currentKey.c_str());
  } else {
    printLine(TFT_YELLOW, "Keyboard: None");
  }

  printLine(TFT_SKYBLUE, "JS_R: %+5.2f %+5.2f", hw.state.joyRX, hw.state.joyRY);
  printLine(TFT_SKYBLUE, "JS_L: %+5.2f %+5.2f", hw.state.joyLX, hw.state.joyLY);
  printLine(TFT_ORANGE, "EncR: %3ld  %s", hw.state.encR, encRBtn ? "Select" : "None");
  printLine(TFT_ORANGE, "EncL: %3ld  %s", hw.state.encL, encLBtn ? "Select" : "None");
  printLine(TFT_GREEN,  "PotL:%+4.1f M:%+4.1f R:%+4.1f", hw.state.potL, hw.state.potM, hw.state.potR);
  
  printLine(TFT_MAGENTA, "AcclX:%+4.1f Y:%+4.1f Z:%+4.1f", hw.state.ax, hw.state.ay, hw.state.az);
  printLine(TFT_MAGENTA, "GyroX:%+4.1f Y:%+4.1f Z:%+4.1f", hw.state.gx, hw.state.gy, hw.state.gz);

  // 7. Push the finished frame to the display
  tft.updateDisplay();
}

void DataView::exit() {
  Serial.println("Leaving Remote Text Mode");
}