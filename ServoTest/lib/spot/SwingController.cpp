#include "SwingController.h"
#include <iostream>

SwingController::SwingController(Configuration *config) {
    spotConfig = config;
}

SwingController::~SwingController() {}

void SwingController::nextFootLocation(float swingProportion, int legIdx, State *state, Command *command, float *newLocation) {
    // todo: assert swing_prop >= 0 and swing_prop <= 1

    float currentFootLocation[3];
    state->getFootLocation(legIdx, currentFootLocation);
    float swingHeight = getSwingHeight(swingProportion);
    float touchDownLocation[3];
    getRaibertTouchdownLocation(legIdx, command, touchDownLocation);
    float timeLeft = spotConfig->dt * spotConfig->swingTicks * (1 - swingProportion);

    float deltaX = (touchDownLocation[0] - currentFootLocation[0]) / timeLeft;
    float deltaY = (touchDownLocation[1] - currentFootLocation[1]) / timeLeft;

    // std::cout << "nextLoc:swingHeight: " << swingHeight << " timeLeft: " << timeLeft << " commandHeight " << command->height << std::endl;

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

    raibertLocation[0] = spotConfig->defaultStance[0][legIdx] + deltaX;
    raibertLocation[1] = spotConfig->defaultStance[1][legIdx] + deltaY;
    raibertLocation[2] = 0.0; 
}