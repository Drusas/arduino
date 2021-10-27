#include "SwingController.h"
#include <iostream>

SwingController::SwingController() {}

SwingController::SwingController(Configuration *config) {
    spotConfig = config;
}

SwingController::~SwingController() {}

void SwingController::configure(Configuration *config) {
    spotConfig = config;
}

void SwingController::nextFootLocation(float swingProportion, int legIdx, State *state, Command *command, float *newLocation) {
    // todo: assert swing_prop >= 0 and swing_prop <= 1

    float currentFootLocation[3];
    state->getFootLocation(legIdx, currentFootLocation);
    // std::cout << "current: [ " << currentFootLocation[0] << ", " << currentFootLocation[1] << ", "<< currentFootLocation[2] << "]" << std::endl;
    float swingHeight = getSwingHeight(swingProportion);
    float touchDownLocation[3];
    getRaibertTouchdownLocation(legIdx, command, touchDownLocation);
    // std::cout << "touchdown:x " << touchDownLocation[0] << " y: " << touchDownLocation[1] << " z: " << touchDownLocation[2] << std::endl;
    float timeLeft = spotConfig->dt * spotConfig->swingTicks * (1 - swingProportion);

    // std::cout << "time_left " << timeLeft << std::endl;

    float deltaX = (touchDownLocation[0] - currentFootLocation[0]) / timeLeft;
    float deltaY = (touchDownLocation[1] - currentFootLocation[1]) / timeLeft;
    // std::cout << "rX: " << touchDownLocation[0] << " curX " << currentFootLocation[0] << std::endl;
    // std::cout << " v: [" << deltaX << " , " << deltaY << ", 0.0000]";

    deltaX *= spotConfig->dt;
    deltaY *= spotConfig->dt;


    // std::cout << " default_foot_location: [" << deltaX << " , " << deltaY << ", 0.0000]" << std::endl;
    
    // std::cout << "nextLoc:swingHeight: " << swingHeight << " timeLeft: " << timeLeft << " commandHeight " << command->height << std::endl;
    // std::cout << "curX: " << currentFootLocation[0] << " delta: " << deltaX << std::endl;
    // std::cout << "curY: " << currentFootLocation[1] << " delta: " << deltaY << std::endl;

    // std::cout << "foot_location: [" << currentFootLocation[0] << ", " << currentFootLocation[1] << ", " << currentFootLocation[2] << "]" << std::endl;

    newLocation[0] = currentFootLocation[0] + deltaX;
    newLocation[1] = currentFootLocation[1] + deltaY;
    newLocation[2] = swingHeight + command->height;
}

float SwingController::getSwingHeight(float swingPhase) {
    // todo: assert swing_prop >= 0 and swing_prop <= 1
    float swingHeight = 0;
    if (swingPhase < 0.5) {
            swingHeight = swingPhase / 0.5 * spotConfig->zClearance;
    }
    else {
        float mult = (1 - (swingPhase -.5) / 0.5);
        swingHeight = spotConfig->zClearance * mult;
    }
    return swingHeight;
}

void SwingController::getRaibertTouchdownLocation(int legIdx, Command *command, float *raibertLocation) {
    float deltaX = spotConfig->alpha * spotConfig->stanceTicks * spotConfig->dt * command->horizontalVelocity[0];
    float deltaY = spotConfig->alpha * spotConfig->stanceTicks * spotConfig->dt * command->horizontalVelocity[1];

    // std::cout << "default: [ " << spotConfig->defaultStance[0][legIdx] << ", " << spotConfig->defaultStance[1][legIdx] << ", " << spotConfig->defaultStance[2][legIdx] << "]";
    // std::cout << " delta: [" << deltaX << " , " << deltaY << "]" << std::endl;
    raibertLocation[0] = spotConfig->defaultStance[0][legIdx] + deltaX;
    raibertLocation[1] = spotConfig->defaultStance[1][legIdx] + deltaY;
    raibertLocation[2] = 0.0; 
    // std::cout << "raibert: [" << raibertLocation[0] << ", " << raibertLocation[1] << "]" << std::endl;
}