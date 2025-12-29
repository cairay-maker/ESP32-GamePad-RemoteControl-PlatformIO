#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <Arduino.h>
#include "hal/TFTHandler.h"
#include "hal/Hardware.h"

/**
 * @brief Base class for all "Worlds" (Games and System Views).
 * Replaces the old "Mode" terminology.
 */
class Activity {
protected:
    TFTHandler& tft;
    Hardware& hw;

public:
    // Constructor handles the references for all child classes
    Activity(TFTHandler& tftRef, Hardware& hwRef) 
        : tft(tftRef), hw(hwRef) {}

    virtual ~Activity() {}

    // Pure virtual functions: Every Game/View MUST implement these
    virtual void init() = 0;   // One-time setup when activity is created
    virtual void update() = 0; // Physics, input, and logic
    virtual void draw() = 0;   // Drawing logic using tft.canvas

    // Optional lifecycle hooks
    virtual void enter() {}    // Called every time you switch TO this activity
    virtual void exit() {}     // Called every time you leave this activity
};

#endif
