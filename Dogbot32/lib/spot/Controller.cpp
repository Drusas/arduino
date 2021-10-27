#include <cstring>
#include "Controller.h"
#include "GaitController.h"
#include "StanceController.h"
#include "SwingController.h"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

Controller::Controller() : spotConfig(nullptr) {
    memset(contactModes, 0, 4*sizeof(uint8_t));
}

Controller::~Controller() {}

void Controller::configure(Configuration *config) {
    spotConfig = config;
    gaitController.configure(config);
    swingController.configure(config);
    stanceController.configure(config);
}

void Controller::stepGait(State *state, Command *command, float newFootLocations[][4], uint8_t *contact_modes) {
    // todo - remove contactModes member ?
    gaitController.getFootContacts(state->ticks, contactModes);
    float newLocation[3];
    for (int i = 0; i < 4; i++) {
        int legContact = contactModes[i];
        if (legContact == 1) {
            stanceController.nextFootLocation(i, state, command, newLocation);
        } 
        else {
            float swingProportion = (float)(gaitController.getSubPhaseTicks(state->ticks)) / spotConfig->swingTicks;
            swingController.nextFootLocation(swingProportion, i, state, command, newLocation);
            // std::cout << std::setprecision(2) << "ticks: " << state->ticks << " leg: " << i << " spt: " << spt << " prop: " << swingProportion << " z: " << newLocation[2] << endl;
        }
        state->setFootLocation(i, newLocation); 
    }

    for (int i = 0; i < 4; i++) {
        contact_modes[i] = contactModes[i];
    }    
    state->ticks++;
    state->getAllFootLocations(newFootLocations);
}