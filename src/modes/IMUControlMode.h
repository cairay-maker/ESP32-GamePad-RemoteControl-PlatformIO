#ifndef IMUCONTROLMODE_H
#define IMUCONTROLMODE_H

#include "modes/Mode.h"
#include "hal/Hardware.h"

class IMUControlMode : public Mode {
public:
    IMUControlMode(TFTHandler& tftRef, Hardware& hwRef);
    void enter() override;
    void update() override;
    void exit() override;

private:
    Hardware& hw;
};

#endif