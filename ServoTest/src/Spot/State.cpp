#include "State.h"

State::State() {
    memset(horizontalVelocity, 0, 2*sizeof(float));
    behaviorState = REST;
    ticks = 0;
    memset(footLocations, 0, 12*sizeof(float));
    memset(jointAngles, 0, 12*sizeof(float));
};

State::~State() {};

// get the x, y, z position of the leg referenced by legIdx
void State::getFootLocation(int legIdx, float *location) {
    for (int i = 0; i < 3; i++) {
        location[i] = footLocations[legIdx][i];
    }
}

float State::getZPosition(int legIdx) {
    return footLocations[legIdx][2];
}