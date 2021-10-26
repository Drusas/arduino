#ifndef _SPOT_CONTROLLER_H
#define _SPOT_CONTROLLER_H

// #include <Arduino.h>
#include "Command.h"
#include "Configuration.h"
#include "GaitController.h"
#include "StanceController.h"
#include "SwingController.h"
#include "State.h"

class Controller {

    
    uint8_t contactModes[4];
    State state;GaitController *gaitController;
    SwingController *swingController;
    StanceController *stanceController;
    
public:
    Configuration *spotConfig;

    Controller();
    ~Controller();
    void stepGait(State *state, Command *command, float new_locations[][4], uint8_t *contact_modes);
};

#endif