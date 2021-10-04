#ifndef _SPOT_COMMAND_H
#define _SPOT_COMMAND_H

// #include <Arduino.h>

class Command {

public:
    float horizontalVelocity[2];
    bool activateEvent;
    bool walkEvent;
    
    Command();
    ~Command();
};

#endif