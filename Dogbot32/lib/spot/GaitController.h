#ifndef _SPOT_GAITCONTROLLER_H
#define _SPOT_GAITCONTROLLER_H

// #include <Arduino.h>
#include "Configuration.h"

class GaitController {

    Configuration *spotConfig;

public:
    GaitController(Configuration * config);
    ~GaitController();
    void getFootContacts(long ticks, uint8_t *modes);
    int getSubPhaseTicks(long ticks);
    int getPhaseIndex(long ticks);
};

#endif