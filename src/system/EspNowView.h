#ifndef ESPNOWVIEW_H
#define ESPNOWVIEW_H

#include "Activity.h"

class EspNowView : public Activity {
public:
    EspNowView(TFTHandler& tft, Hardware& hw);

    // Add these to satisfy the pure virtual requirements from Activity
    void init() override;   
    void enter() override;
    void update() override;
    void draw() override;   // Ensure this matches the virtual signature
    void exit() override;

private:
    unsigned long lastPrintTime = 0;
    unsigned long lastDisplayedCount = 0;
};

#endif