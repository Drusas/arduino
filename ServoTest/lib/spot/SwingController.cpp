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

    std::cout << "nextLoc:swingHeight: " << swingHeight << " timeLeft: " << timeLeft << " commandHeight " << command->height << std::endl;

    newLocation[0] = currentFootLocation[0] + deltaX;
    newLocation[1] = currentFootLocation[1] + deltaY;
    newLocation[2] = swingHeight + command->height;
    // float v = ()
    
    // foot_location = state.foot_locations[:, leg_index]
    // swing_height_ = self.swing_height(swing_prop)
    // touchdown_location = self.raibert_touchdown_location(leg_index, command)
    // time_left = self.config.dt * self.config.swing_ticks * (1.0 - swing_prop)
    // v = (touchdown_location - foot_location) / time_left * np.array([1, 1, 0])
    // delta_foot_location = v * self.config.dt
    // z_vector = np.array([0, 0, swing_height_ + command.height])
    // return foot_location * np.array([1, 1, 0]) + z_vector + delta_foot_location
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