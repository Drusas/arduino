#ifndef _SPOT_SWINGCONTROLLER_H
#define _SPOT_SWINGCONTROLLER_H

#include <Arduino.h>
#include "Command.h"
#include "Configuration.h"
#include "State.h"

class SwingController {

    Configuration *spotConfig;
    float getSwingHeight(float swingPhase);
    void getRaibertTouchdownLocation(int legIdx, Command *command, float *raibertLocation);
    
public:
    SwingController(Configuration *config);
    ~SwingController();
    void nextFootLocation(float swingProportion, int legIdx, State *state, Command *command, float *newLocation);
};

#endif