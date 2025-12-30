#ifndef SYSTEMMENU_H
#define SYSTEMMENU_H

#include "Activity.h"
#include "system/Axis3DView.h"   // Updated path to match typical view structure
#include "system/DataView.h"
#include "system/GraphicView.h"

class SystemMenu : public Activity {
public:
  explicit SystemMenu(TFTHandler& tft, Hardware& hw);

  void init() override {} 
  void draw() override {} 
  void enter() override;
  void update() override;
  void exit() override;

  // Made public so update() can call it for live WiFi refreshes
  void drawMenu();

private:
  enum SystemState { MENU, AXIS, DATA, GRAPHIC };
  SystemState currentState = MENU;

  void startApp(int index);

  // App Selection
  int selectedApp = 0;
  const int numApps = 3;
  const char* appNames[3] = {"3D AXIS VISUALIZER", "LIVE DATA TELEMETRY", "GRAPHIC DASHBOARD"};

  // Selection & Timing Logic
  unsigned long selectHoldStartTime = 0;
  bool isHoldingSelect = false;
  bool suppressNextRelease = false;
  
  // WiFi Status tracking for the top-right icon
  bool lastWifiState = false; 

  // System View instances
  Axis3DView axis3DView;
  DataView dataView;
  GraphicView graphicView;
};

#endif