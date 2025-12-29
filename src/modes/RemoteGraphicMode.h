#ifndef REMOTEGRAPHICMODE_H
#define REMOTEGRAPHICMODE_H

#include "modes/Mode.h"
#include "hal/Hardware.h"

class RemoteGraphicMode : public Mode {
public:
  explicit RemoteGraphicMode(TFTHandler& tft, Hardware& hw);
  void enter() override;
  void update() override;
  void exit() override;

private:
  Hardware& hw; // [CHANGE] Unified reference
};

#endif