#include "Disabled.h"
#include "Idle.h"
#include <iostream>
#include "Sitting.h"
#include "Standing.h"

void Idle::entry() {
    std::cout << "state:idle" << std::endl;
}

void Idle::react(ToStand const &event) {
    std::cout << "state:idle::react" << std::endl;
    if (event.q != nullptr) {
        event.q->stand();
        transit<Standing>();
    }
}

void Idle::react(ToSit const &event) {
    std::cout << "state:idle::react" << std::endl;
    if (event.q != nullptr) {
        event.q->sit();
        transit<Sitting>();
    }
}

void Idle::react(ToDisable const &event) {
    transit<Disabled>();
}