#include "Configuration.h"

Configuration::Configuration() {
    
}

void Configuration::getDefaultStanceLocaton(int legIdx, float *location) {
    for (int i = 0; i < 3; i++) {
        location[i] = defaultStance[legIdx][i];
    }
}

