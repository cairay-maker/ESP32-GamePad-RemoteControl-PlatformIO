#ifndef GAMEPADMODE2_H
#define GAMEPADMODE2_H

#include "Mode.h"
#include "Hardware.h"

class GamePadMode2 : public Mode {  // ← Renamed class
public:
  explicit GamePadMode2(TFTHandler& tft, Hardware& hw);
  void enter() override;
  void update() override;
  void exit() override;
private:
  Hardware& hw;
};

#endif