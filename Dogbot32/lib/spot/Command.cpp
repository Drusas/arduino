#include "Command.h"
#include <cstring>

Command::Command() {
    memset(horizontalVelocity, 0, 2*sizeof(float));
    activateEvent = walkEvent = false;
    height = -0.16;
};

Command::~Command() {};