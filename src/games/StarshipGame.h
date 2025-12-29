#ifndef STARSHIP_GAME_H
#define STARSHIP_GAME_H

#include "Activity.h" // Updated include

class StarshipGame : public Activity { // Added inheritance
public:
    StarshipGame(TFTHandler& tftRef, Hardware& hwRef);
    
    // Added override keywords
    void init() override;
    void update() override;
    void draw() override;
    
    void cleanup();
    bool isRunning() const { return running; }

private:
    // REMOVED: TFTHandler& tft; <-- Inherited from Activity
    // REMOVED: Hardware& hw;   <-- Inherited from Activity
    
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