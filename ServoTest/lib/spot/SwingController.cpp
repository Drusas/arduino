#include "SwingController.h"

SwingController::SwingController(Configuration *config) {
    spotConfig = config;
}

SwingController::~SwingController() {}

void SwingController::nextFootLocation(float swingProportion, int legIdx, State *state, Command *command, float *newLocation) {
    // todo: assert swing_prop >= 0 and swing_prop <= 1

    float currentFootLocation[3];
    state->getFootLocation(legIdx, currentFootLocation);
    // float swingHeight = getSwingHeight(swingProportion);
    // float touchDownLocation[2];
    // getRaibertTouchdownLocation(legIdx, command, touchDownLocation);
    // float timeLeft = spotConfig->dt * spotConfig->swingTicks * (1 - swingProportion);
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
            swingHeight = spotConfig->zClearance * (1 - (swingPhase - 0.5) / 0.5);
    }
    return swingHeight;
}

void SwingController::getRaibertTouchdownLocation(int legIdx, Command *command, float *raibertLocation) {
    // float deltaX = spotConfig->alpha * spotConfig->stanceTicks * spotConfig->dt * command->horizontalVelocity[0];
    // float deltaY = spotConfig->alpha * spotConfig->stanceTicks * spotConfig->dt * command->horizontalVelocity[1];
 
    // delta_p_2d = (
    //         self.config.alpha
    //         * self.config.stance_ticks
    //         * self.config.dt
    //         * command.horizontal_velocity
    //     )
    //     delta_p = np.array([delta_p_2d[0], delta_p_2d[1], 0])

    //     return self.config.default_stance[:, leg_index] + delta_p
}