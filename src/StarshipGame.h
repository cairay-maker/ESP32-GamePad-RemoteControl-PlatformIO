#ifndef STARSHIP_GAME_H
#define STARSHIP_GAME_H

#include "Mode.h"
#include "Hardware.h"
#include <TFT_eSPI.h>

class StarshipGame : public Mode {
public:
    StarshipGame(TFTHandler& tftRef, Hardware& hwRef);
    void init();
    void cleanup();
    
    // Add these for GamePadMode compatibility
    void draw() {}             // Does nothing because update() handles it
    bool isRunning() { return true; } // Always returns true for now

    void enter() override;
    void update() override;
    void exit() override;

private:
    Hardware& hw;
    TFT_eSprite sprite;
    
    enum GameState { WAITING, PLAYING, GAMEOVER };
    GameState state;

    float pitchOffset = 0, rollOffset = 0;
    float obstacleZ = 100.0f;
    float obsX = 0, obsY = 0;
    int score = 0;

    void resetObstacle();
    std::pair<int, int> project(float x, float y, float z);
};

#endif