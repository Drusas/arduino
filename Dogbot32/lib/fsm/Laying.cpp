#include "Disabled.h"
#include "Idle.h"
#include "Laying.h"
#include "Sitting.h"
#include "Standing.h"
#include <iostream>

void Laying::entry() {
    std::cout << "state:laying" << std::endl;
}

void Laying::react(ToIdle const &event) {
    transit<Idle>();
}

void Laying::react(ToStand const &event) {
    if (event.q != nullptr) {
        event.q->stand();
    }
    transit<Standing>();
}

void Laying::react(ToSit const &event) {
    if (event.q != nullptr) {
        event.q->sit();
    }
    transit<Sitting>();
}

void Laying::react(ToDisable const &event) {
    transit<Disabled>();
}