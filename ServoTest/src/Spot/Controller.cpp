#include "Controller.h"
#include "GaitController.h"
#include "StanceController.h"
#include "SwingController.h"
#include "Utils.h"

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

void Controller::stepGait(State *state, Command command, float* new_locations, uint8_t *contact_modes) {
    TRACE("state: %d, command: %d, locations: %d, modes: %d\n",state, command, new_locations, contact_modes);

    // gaitController->getFootContacts(contactModes);
}