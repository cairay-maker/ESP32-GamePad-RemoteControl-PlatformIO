#ifndef SYSTEMMENU_H
#define SYSTEMMENU_H

#include "Activity.h"
#include "system/Axis3DView.h"
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

private:
  unsigned long selectHoldStartTime = 0;
  bool isHoldingSelect = false;
  bool suppressNextRelease = false;

  // System View instances
  Axis3DView axis3DView;
  DataView dataView;
  GraphicView graphicView;

  enum SystemState { MENU, AXIS, DATA, GRAPHIC };
  SystemState currentState = MENU;

  int selectedApp = 0;
  const int numApps = 3;
  const char* appNames[3] = {"3D AXIS VISUALIZER", "LIVE DATA TELEMETRY", "GRAPHIC DASHBOARD"};

  void drawMenu();
  void startApp(int index);
};

#endif