#ifndef DataView_H
#define DataView_H

#include "Activity.h"

class DataView : public Activity {
public:
  explicit DataView(TFTHandler& tft, Hardware& hw);

  // Mark all as override; added init/draw to satisfy base class requirements
  void init() override {}
  void draw() override {}
  void enter() override;
  void update() override;
  void exit() override;

private:
  // REMOVED: Hardware& hw;  <-- Now using protected member from Activity.h
};

#endif