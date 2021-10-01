#include "Command.h"

Command::Command() {
    memset(horizontalVelocity, 0, 2*sizeof(float));
    activateEvent = walkEvent = false;
};

Command::~Command() {};