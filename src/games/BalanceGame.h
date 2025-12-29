#ifndef BALANCE_GAME_H
#define BALANCE_GAME_H

#include "Activity.h"
#include <utility> 

class BalanceGame : public Activity {
public:
    BalanceGame(TFTHandler& tftRef, Hardware& hwRef);
    
    // Use override to satisfy Activity.h requirements
    void init() override;           
    void update() override;
    void draw() override;            
    void enter() override;
    void exit() override;
    
private:
    // REMOVED: Hardware& hw; <-- Inherited from Activity.h

    // Simulation Variables - Beam 1
    float ballX, ballVelocity, beamAngle;      

    // Simulation Variables - Beam 2
    float ballX2, ballVelocity2, beamAngle2;

    bool isReleased; 

    // PID Constants
    const float Kp = 0.85f;
    const float Ki = 0.02f;
    const float Kd = 0.45f;

    float dynamicKp, dynamicKi, dynamicKd;
    
    // PID State
    float errorPrior;
    float integral;
    float errorPrior2;
    float integral2;

    std::pair<int, int> project(float x, float y, float z);
    void resetSim();
};

#endif