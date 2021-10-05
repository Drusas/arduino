#ifndef _SPOT_GAITCONTROLLER_H
#define _SPOT_GAITCONTROLLER_H

// #include <Arduino.h>
#include "Configuration.h"

class GaitController {

    Configuration *spotConfig;

    int getPhaseIndex(int ticks);

public:
    GaitController(Configuration * config);
    ~GaitController();
    void getFootContacts(int ticks, uint8_t *modes);
    int getSubPhaseTicks(int ticks);
};

#endif