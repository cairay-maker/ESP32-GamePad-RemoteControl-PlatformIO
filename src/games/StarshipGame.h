#ifndef STARSHIP_GAME_H
#define STARSHIP_GAME_H

#include "hal/TFTHandler.h"
#include "hal/Hardware.h"

class StarshipGame {
public:
    StarshipGame(TFTHandler& tftRef, Hardware& hwRef);
    void init();
    void update();
    void draw();
    void cleanup();
    bool isRunning() const { return running; }

private:
    TFTHandler& tft;
    Hardware& hw;
    bool running = true;

    struct Obstacle {
        float x, y, z;
        bool active;
    };

    #define MAX_STARSHIP_OBS 3
    Obstacle obs[MAX_STARSHIP_OBS];
    
    int score = 0;
    float speed = 2.5f;
    unsigned long lastSpeedUp = 0;

    struct Point2D { int x, y, size; };
    Point2D project(float x, float y, float z);
    void spawnObstacle(int index);
    void endGame();
};
#endif