#ifndef _SPOT_STANCECONTROLLER_H
#define _SPOT_STANCECONTROLLER_H

#include <Arduino.h>
#include "Configuration.h"

class StanceController {

    Configuration *spotConfig;
    
public:
    StanceController(Configuration *config);
    ~StanceController();
};

#endif