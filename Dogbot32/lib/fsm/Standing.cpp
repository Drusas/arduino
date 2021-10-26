#include "Disabled.h"
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
    if (event.q != nullptr) {
        event.q->walk();
    }
    transit<Walking>();
}

void Standing::react(ToDisable const &event) {
    transit<Disabled>();
}