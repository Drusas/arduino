#include "Configuration.h"

Configuration::Configuration() {
    memset(contactPhases, 0, 8*4*sizeof(uint8_t));
}

