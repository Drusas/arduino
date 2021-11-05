#include "State.h"
#include <algorithm>
#include <cstring>
#include <iostream>

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
        location[i] = footLocations[i][legIdx];
    }
}

void State::setFootLocation(int legIdx, float *location) {
    for (int i = 0; i < 3; i++) {
        footLocations[i][legIdx] = location[i];
    }
}

void State::setAllFootLocations(float locations[][4]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            footLocations[i][j] = locations[i][j];
        }
    }
}

void State::getAllFootLocations(float dest[][4]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            dest[i][j] = footLocations[i][j];
        }
    }
}

void State::printfootLocations() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << "[" << i << "][" << j << "]: " << footLocations[i][j] << std::endl;
        }
    }
}

float State::getZPosition(int legIdx) {
    // std::cout << "State:getZPosition(" << legIdx << "): " << footLocations[2][legIdx] << std::endl;
    return footLocations[2][legIdx];
}

