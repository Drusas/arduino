#include <cstring>
#include "Controller.h"
#include "GaitController.h"
#include "StanceController.h"
#include "SwingController.h"

Controller::Controller() {
    spotConfig = new Configuration();
    memset(contactModes, 0, 4*sizeof(uint8_t));

    gaitController = new GaitController(spotConfig);
    stanceController = new StanceController(spotConfig);
    swingController = new SwingController(spotConfig);
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