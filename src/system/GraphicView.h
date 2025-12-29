#ifndef GraphicView_H
#define GraphicView_H

#include "Activity.h"

class GraphicView : public Activity {
public:
  explicit GraphicView(TFTHandler& tft, Hardware& hw);
  
  // Mandatory Activity Interface
  void init() override {}
  void draw() override {}
  void enter() override;
  void update() override;
  void exit() override;

private:
  // REMOVED: Hardware& hw; <-- Inherited from Activity.h
};

#endif