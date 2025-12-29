#include "games/BalanceGame.h"
#include <Arduino.h>

static const int SCREEN_CENTER_X = 80; 
static const int SCREEN_CENTER_Y = 64; 

BalanceGame::BalanceGame(TFTHandler& tftRef, Hardware& hwRef)
    : Activity(tftRef, hwRef) {} 

void BalanceGame::init() {
    resetSim();
}

void BalanceGame::resetSim() {
    ballX = 0; ballVelocity = 0; beamAngle = 0;
    ballX2 = 0; ballVelocity2 = 0; beamAngle2 = 0;
    integral = 0; errorPrior = 0;
    integral2 = 0; errorPrior2 = 0;
    isReleased = false;
}

void BalanceGame::enter() {
    init();
}

std::pair<int, int> BalanceGame::project(float x, float y, float z) {
    float focalLength = 100.0f;
    int screenX = (int)((x * focalLength) / (z + 100.0f)) + SCREEN_CENTER_X;
    int screenY = (int)((y * focalLength) / (z + 100.0f)) + SCREEN_CENTER_Y; 
    return {screenX, screenY};
}

void BalanceGame::update() {
    // Uses 'hw' inherited from Activity
    float multP = 1.0f + hw.state.potL; 
    float multI = 1.0f + hw.state.potM; 
    float multD = 1.0f + hw.state.potR; 

    dynamicKp = Kp * multP;
    dynamicKi = Ki * multI;
    dynamicKd = Kd * multD;

    float joyX = hw.state.joyRX; 
    
    if (abs(joyX) > 0.05f) {
        isReleased = false; 
        ballX += (joyX * 3.0f); 
        ballX2 = ballX;      
        ballVelocity = 0; 
        ballVelocity2 = 0;
        integral = 0; integral2 = 0;     
    } else {
        isReleased = true;
    }

    ballX = constrain(ballX, -55.0f, 55.0f);
    ballX2 = constrain(ballX2, -55.0f, 55.0f);

    if (isReleased) {
        float error1 = 0 - ballX; 
        integral = constrain(integral + error1, -50, 50);
        float deriv1 = error1 - errorPrior;
        beamAngle = (Kp * error1) + (Ki * integral) + (Kd * deriv1);
        beamAngle = constrain(beamAngle, -25.0f, 25.0f);
        errorPrior = error1;

        float error2 = 0 - ballX2; 
        integral2 = constrain(integral2 + error2, -50, 50);
        float deriv2 = error2 - errorPrior2;
        beamAngle2 = (dynamicKp * error2) + (dynamicKi * integral2) + (dynamicKd * deriv2);
        beamAngle2 = constrain(beamAngle2, -25.0f, 25.0f);
        errorPrior2 = error2;

        float rad1 = beamAngle * (M_PI / 180.0f);
        ballVelocity += sin(rad1) * 0.35f;
        ballVelocity *= 0.98f; 
        ballX += ballVelocity;

        float rad2 = beamAngle2 * (M_PI / 180.0f);
        ballVelocity2 += sin(rad2) * 0.35f;
        ballVelocity2 *= 0.98f; 
        ballX2 += ballVelocity2;
    } else {
        beamAngle = 0; beamAngle2 = 0;
        errorPrior = 0; errorPrior2 = 0;
    }
}

void BalanceGame::draw() {
    // Uses 'tft' inherited from Activity
    TFT_eSprite& c = tft.canvas;
    
    float beamHalfLength = 55.0f;

    // Beam 1
    float rad1 = beamAngle * (M_PI / 180.0f);
    auto pStart1 = project(-cos(rad1)*beamHalfLength, -sin(rad1)*beamHalfLength, 0);
    auto pEnd1   = project(cos(rad1)*beamHalfLength, sin(rad1)*beamHalfLength, 0);
    c.drawLine(pStart1.first, pStart1.second, pEnd1.first, pEnd1.second, TFT_WHITE);
    
    auto pBall1 = project(cos(rad1)*ballX, (sin(rad1)*ballX)-6, 0);
    c.fillCircle(pBall1.first, pBall1.second, 4, TFT_CYAN);

    // Beam 2
    float rad2 = beamAngle2 * (M_PI / 180.0f);
    int yOff2 = 30; 
    auto pStart2 = project(-cos(rad2)*beamHalfLength, -sin(rad2)*beamHalfLength, 0);
    auto pEnd2   = project(cos(rad2)*beamHalfLength, sin(rad2)*beamHalfLength, 0);
    c.drawLine(pStart2.first, pStart2.second + yOff2, pEnd2.first, pEnd2.second + yOff2, TFT_WHITE);
    
    auto pBall2 = project(cos(rad2)*ballX2, (sin(rad2)*ballX2)-6, 0);
    c.fillCircle(pBall2.first, pBall2.second + yOff2, 4, TFT_GREEN);

    // HUD
    c.setTextSize(1);
    c.setTextColor(TFT_CYAN);
    c.setCursor(10, 5);
    c.printf("Raw P:%4.2f I:%4.2f D:%4.2f", Kp, Ki, Kd);

    c.setCursor(10, 15);
    c.setTextColor(TFT_GREEN);
    c.printf("Adj P:%4.2f I:%4.2f D:%4.2f", dynamicKp, dynamicKi, dynamicKd);

    c.setCursor(10, 25);
    c.setTextColor(TFT_ORANGE);
    c.printf("Ang: %+4.1f vs %+4.1f", beamAngle, beamAngle2);

    c.setTextColor(TFT_RED);
    c.drawCentreString("R.Joy + Pots to Adj", 80, 118, 1);
}

void BalanceGame::exit() {}