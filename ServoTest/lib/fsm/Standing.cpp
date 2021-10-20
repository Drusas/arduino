#include "Idle.h"
#include "Standing.h"
#include "Walking.h"
#include <iostream>

void Standing::entry() {
    std::cout << "state:standing" << std::endl;
}

void Standing::react(ToIdle const &event) {
        transit<Idle>();
}

void Standing::react(ToWalk const &event) {
        transit<Walking>();
}