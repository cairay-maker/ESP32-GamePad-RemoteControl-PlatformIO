#ifndef Axis3DView_H
#define Axis3DView_H

#include "Activity.h"

class Axis3DView : public Activity {
public:
    Axis3DView(TFTHandler& tftRef, Hardware& hwRef);
    
    // Adding override keywords to ensure they match the base class
    void init() override {} 
    void draw() override {} 
    void enter() override;
    void update() override;
    void exit() override;

private:
    // REMOVED: Hardware& hw; <-- Inherited from Activity.h
};

#endif