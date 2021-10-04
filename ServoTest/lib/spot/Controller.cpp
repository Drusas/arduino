#include "Controller.h"
#include "GaitController.h"
#include "StanceController.h"
#include "SwingController.h"
// #include "Utils.h"

Controller::Controller() {
    spotConfig = new Configuration();
}

Controller::Controller(Configuration *config) {
    spotConfig = config;
    memset(contactModes, 0, 4*sizeof(uint8_t));

    gaitController = new GaitController(config);
    stanceController = new StanceController(config);
    swingController = new SwingController(config);
}

Controller::~Controller() {
    delete gaitController;
    gaitController = 0;

    delete stanceController;
    stanceController = 0;

    delete swingController;
    swingController = 0;
}

void Controller::stepGait(State *state, Command *command, float* newFootLocations, uint8_t *contact_modes) {
    // TRACE("state: %d, command: %d, locations: %d, modes: %d\n",state, command, (int)(newFootLocations), contact_modes);
    
    gaitController->getFootContacts(state->ticks, contactModes);
    float newLocation[3];
    for (int i = 0; i < 4; i++) {
        int legContact = contactModes[i];
        if (legContact == 1) {
            stanceController->nextFootLocation(i, state, command, newLocation);
        } 
        else {
            float swingProportion = gaitController->getSubPhaseTicks(state->ticks) / spotConfig->swingTicks;
            swingController->nextFootLocation(swingProportion, i, state, command, newLocation);
        } 
    }
}