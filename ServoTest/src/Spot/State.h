#ifndef _SPOT_STATE_H
#define _SPOT_STATE_H

#include <Arduino.h>

enum BehaviorState {
    REST = 0,
    WALK = 1,
};

class State {

public:
    float horizontalVelocity[2];
    BehaviorState behaviorState;
    long ticks;
    float footLocations[3][4];
    float jointAngles[3][4];

    State();
    ~State();
};

#endif