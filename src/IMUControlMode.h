#ifndef IMUCONTROLMODE_H
#define IMUCONTROLMODE_H

#include "Mode.h"
#include "Hardware.h"

class IMUControlMode : public Mode {
public:
    explicit IMUControlMode(TFTHandler& tft, Hardware& hw);

    void enter() override;
    void update() override; // One function to rule them all
    void exit() override;

private:
    Hardware& hw;
    TFT_eSprite sprite;
};

#endif