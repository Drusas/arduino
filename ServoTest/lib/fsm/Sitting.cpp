#include "Disabled.h"
#include "Idle.h"
#include "Sitting.h"
#include "Standing.h"
#include <iostream>

void Sitting::entry() {
    std::cout << "state:standing" << std::endl;
}

void Sitting::react(ToIdle const &event) {
    transit<Idle>();
}

void Sitting::react(ToStand const &event) {
    if (event.q != nullptr) {
        event.q->stand();
    }
    transit<Standing>();
}

void Sitting::react(ToDisable const &event) {
    transit<Disabled>();
}