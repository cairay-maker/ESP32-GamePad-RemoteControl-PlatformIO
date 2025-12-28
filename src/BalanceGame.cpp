#include "BalanceGame.h"
#include <Arduino.h>

static const int FRAME_W = 140;
static const int FRAME_H = 120;
static const int CENTER_X = 70;
static const int CENTER_Y = 60; 

BalanceGame::BalanceGame(TFTHandler& tftRef, Hardware& hwRef)
    : Mode(tftRef), hw(hwRef), sprite(&tftRef.getTFT()) {}

void BalanceGame::init() {
    if (!sprite.created()) sprite.createSprite(FRAME_W, FRAME_H);
    resetSim();
}

void BalanceGame::resetSim() {
    ballX = 0; ballVelocity = 0; beamAngle = 0;
    ballX2 = 0; ballVelocity2 = 0; beamAngle2 = 0;
    integral = 0; errorPrior = 0;
    integral2 = 0; errorPrior2 = 0;
    isReleased = false;
}

std::pair<int, int> BalanceGame::project(float x, float y, float z) {
    float focalLength = 100.0f;
    int screenX = (int)((x * focalLength) / (z + 100.0f)) + CENTER_X;
    int screenY = (int)((y * focalLength) / (z + 100.0f)) + CENTER_Y;
    return {screenX, screenY};
}

void BalanceGame::enter() {
    tft.clearScreen();
    init();
}

void BalanceGame::update() {
    hw.joyRight.update(); 
    
    // Calculate Adjusted Gains for Beam 2
    float multP = 1.0f + hw.potLeft.getNormalized(); 
    float multI = 1.0f + hw.potMid.getNormalized(); 
    float multD = 1.0f + hw.potRight.getNormalized(); 

    dynamicKp = Kp * multP;
    dynamicKi = Ki * multI;
    dynamicKd = Kd * multD;

    float joyX = hw.joyRight.getMappedX(); 
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
        // --- BEAM 1 (Fixed PID - RAW) ---
        float error1 = 0 - ballX; 
        integral = constrain(integral + error1, -50, 50);
        float deriv1 = error1 - errorPrior;
        // USES Kp, Ki, Kd
        beamAngle = (Kp * error1) + (Ki * integral) + (Kd * deriv1);
        beamAngle = constrain(beamAngle, -25.0f, 25.0f);
        errorPrior = error1;

        // --- BEAM 2 (Tuned PID - ADJUSTED) ---
        float error2 = 0 - ballX2; 
        integral2 = constrain(integral2 + error2, -50, 50);
        float deriv2 = error2 - errorPrior2;
        // USES dynamicKp, dynamicKi, dynamicKd
        beamAngle2 = (dynamicKp * error2) + (dynamicKi * integral2) + (dynamicKd * deriv2);
        beamAngle2 = constrain(beamAngle2, -25.0f, 25.0f);
        errorPrior2 = error2;

        // --- Physics ---
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
    sprite.fillSprite(TFT_BLACK);

    float beamHalfLength = 55.0f;

    // Draw Beam 1 (Cyan - Raw)
    float rad1 = beamAngle * (PI / 180.0f);
    auto pStart1 = project(-cos(rad1)*beamHalfLength, -sin(rad1)*beamHalfLength, 0);
    auto pEnd1   = project(cos(rad1)*beamHalfLength, sin(rad1)*beamHalfLength, 0);
    sprite.drawLine(pStart1.first, pStart1.second, pEnd1.first, pEnd1.second, TFT_WHITE);
    auto pBall1 = project(cos(rad1)*ballX, (sin(rad1)*ballX)-6, 0);
    sprite.fillCircle(pBall1.first, pBall1.second, 4, TFT_CYAN);

    // Draw Beam 2 (Green - Adjusted)
    float rad2 = beamAngle2 * (PI / 180.0f);
    int yOff2 = 30; 
    auto pStart2 = project(-cos(rad2)*beamHalfLength, -sin(rad2)*beamHalfLength, 0);
    auto pEnd2   = project(cos(rad2)*beamHalfLength, sin(rad2)*beamHalfLength, 0);
    sprite.drawLine(pStart2.first, pStart2.second + yOff2, pEnd2.first, pEnd2.second + yOff2, TFT_WHITE);
    auto pBall2 = project(cos(rad2)*ballX2, (sin(rad2)*ballX2)-6, 0);
    sprite.fillCircle(pBall2.first, pBall2.second + yOff2, 4, TFT_GREEN);

    // --- HUD ---
    sprite.setTextSize(1);
    sprite.setTextColor(TFT_CYAN);
    sprite.setCursor(2, 2);
    sprite.printf("RawP:%4.2f I:%4.2f D:%4.2f", Kp, Ki, Kd); // Top Label

    sprite.setCursor(2, 12);
    sprite.setTextColor(TFT_GREEN);
    sprite.printf("AdjP:%4.2f I:%4.2f D:%4.2f", dynamicKp, dynamicKi, dynamicKd); // Bottom Label

    sprite.setCursor(2, 22);
    sprite.setTextColor(TFT_ORANGE);
    sprite.printf("Ang: %+4.1f vs %+4.1f", beamAngle, beamAngle2);

    sprite.setTextColor(TFT_RED);
    sprite.setTextDatum(BC_DATUM); 
    sprite.drawString("R.Joy + Pots to Adj", FRAME_W / 2, FRAME_H - 2 );
    
    sprite.pushSprite(10, 10);
}

void BalanceGame::exit() { cleanup(); }
void BalanceGame::cleanup() { if (sprite.created()) sprite.deleteSprite(); }