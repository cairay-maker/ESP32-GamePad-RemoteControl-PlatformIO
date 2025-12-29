#ifndef REMOTETEXTMODE_H
#define REMOTETEXTMODE_H

#include "modes/Mode.h"
#include "hal/Hardware.h"

class RemoteTextMode : public Mode {
public:
  explicit RemoteTextMode(TFTHandler& tft, Hardware& hw);

  void enter() override;
  void update() override;
  void exit() override;

private:
  Hardware& hw; // [CHANGE] Only need the master hardware reference now
};

#endif