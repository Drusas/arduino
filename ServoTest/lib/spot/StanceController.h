#ifndef _SPOT_STANCECONTROLLER_H
#define _SPOT_STANCECONTROLLER_H

// #include <Arduino.h>
#include "Command.h"
#include "Configuration.h"
#include "State.h"

class StanceController {

    Configuration *spotConfig;
    void getPositionDelta(int legIdx, State *state, Command *command, float *delta);

public:
    StanceController(Configuration *config);
    ~StanceController();
    void nextFootLocation(int legIdx, State *state, Command *command, float* nextLocation);
};

#endif