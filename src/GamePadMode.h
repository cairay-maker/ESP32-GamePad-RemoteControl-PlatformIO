#ifndef GAMEPADMODE_H
#define GAMEPADMODE_H

#include "Mode.h"

class GamePadMode : public Mode {
public:
  explicit GamePadMode(TFTHandler& tft) : Mode(tft) {}
  void enter() override;
  void update() override;
  void exit() override;
};

#endif