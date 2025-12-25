#ifndef MODE_H
#define MODE_H

#include "TFTHandler.h"

class Mode {
public:
  explicit Mode(TFTHandler& tft) : tft(tft) {}
  virtual ~Mode() = default;

  virtual void enter() = 0;
  virtual void update() = 0;     // Called every loop
  virtual void exit() {}

protected:
  TFTHandler& tft;
};

#endif