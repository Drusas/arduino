#include "StanceController.h"
#include <iostream>

StanceController::StanceController(Configuration *config) {
    spotConfig = config;
}

StanceController::~StanceController() {}

void StanceController::nextFootLocation(int legIdx, State *state, Command *command, float* nextLocation) {
    // std::cout << "nextfootlocation:horz: " << command->horizontalVelocity[0] << " dt: " << spotConfig->dt << std::endl;
    float currentFootLocation[3];
    state->getFootLocation(legIdx, currentFootLocation); 
    float delta[3];
    getPositionDelta(legIdx, state, command, delta);
    // std::cout << "nextfootlocation:dx: " << delta[0] << " dy: " << delta[1] << " dz: " << delta[2] << std::endl;
    // std::cout << "foot_location [" << currentFootLocation[0] << ", " << currentFootLocation[1] << ", " << currentFootLocation[2] << "]";
    // std::cout << " delta_p [" << delta[0] << ", " << delta[1] << ", " << delta[2] << "]" << std::endl;
    nextLocation[0] = currentFootLocation[0] + delta[0];
    nextLocation[1] = currentFootLocation[1] + delta[1];
    nextLocation[2] = currentFootLocation[2] + delta[2];
}

void StanceController::getPositionDelta(int legIdx, State *state, Command *command, float *delta) {
    float zPos = state->getZPosition(legIdx);
    zPos  = 1.0 / spotConfig->zTimeConstant * (state->height - zPos);
    zPos *= spotConfig->dt;
    
    float x = -command->horizontalVelocity[0] * spotConfig->dt;
    float y = -command->horizontalVelocity[1] * spotConfig->dt;
    
    // std::cout << "horz: " << command->horizontalVelocity[0] << " dt: " << spotConfig->dt << std::endl;
    delta[0] = x;
    delta[1] = y;
    delta[2] = zPos;
}