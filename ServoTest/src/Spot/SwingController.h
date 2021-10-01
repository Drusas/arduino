#ifndef _SPOT_SWINGCONTROLLER_H
#define _SPOT_SWINGCONTROLLER_H

#include <Arduino.h>
#include "Configuration.h"

class SwingController {

    Configuration *spotConfig;
    
public:
    SwingController(Configuration *config);
    ~SwingController();
};

#endif