#include "State.h"

State::State() {
    memset(horizontalVelocity, 0, 2*sizeof(float));
    behaviorState = REST;
    ticks = 0;
    memset(footLocations, 0, 12*sizeof(float));
    memset(jointAngles, 0, 12*sizeof(float));
};

State::~State() {};