#ifndef BALANCE_GAME_H
#define BALANCE_GAME_H

#include "Mode.h"
#include "Hardware.h"
#include <TFT_eSPI.h>
#include <utility> 

class BalanceGame : public Mode {
public:
    BalanceGame(TFTHandler& tftRef, Hardware& hwRef);
    
    void init();           
    void update() override;
    void draw();            
    void enter() override;
    void exit() override;
    
    void cleanup();
    bool isRunning() { return true; }

private:
    Hardware& hw;
    TFT_eSprite sprite;

    // Simulation Variables - Beam 1
    float ballX, ballVelocity, beamAngle;      

    // Simulation Variables - Beam 2
    float ballX2, ballVelocity2, beamAngle2;

    bool isReleased; 

    // PID Constants (Gold Standard)
    const float Kp = 0.85f;
    const float Ki = 0.02f;
    const float Kd = 0.45f;

    float dynamicKp, dynamicKi, dynamicKd;
    
    // PID State - Beam 1 (Uses Raw Kp, Ki, Kd)
    float errorPrior;
    float integral;

    // PID State - Beam 2 (Uses Dynamic Gains)
    float errorPrior2;
    float integral2;

    std::pair<int, int> project(float x, float y, float z);
    void resetSim();
};

#endif