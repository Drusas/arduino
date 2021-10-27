#ifndef _SPOT_SWINGCONTROLLER_H
#define _SPOT_SWINGCONTROLLER_H

// #include <Arduino.h>
#include "Command.h"
#include "Configuration.h"
#include "State.h"

class SwingController {

    Configuration *spotConfig;
    
    
    
public:
    SwingController();
    SwingController(Configuration *config);
    ~SwingController();

    void configure(Configuration *config);
    void getRaibertTouchdownLocation(int legIdx, Command *command, float *raibertLocation);
    float getSwingHeight(float swingPhase);
    void nextFootLocation(float swingProportion, int legIdx, State *state, Command *command, float *newLocation);
};

#endif