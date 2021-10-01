#ifndef _SPOT_CONFIGURATION_H
#define _SPOT_CONFIGURATION_H

#include <Arduino.h>

class Configuration {

public:
    uint8_t contactPhases[8][4];
    int phaseLength;

    Configuration();
    ~Configuration();
    
};

#endif