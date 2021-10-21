#include "Disabled.h"
#include "Enabled.h"
#include "Idle.h"
#include <iostream>


void Enabled::entry() {
    std::cout << "state:enabled" << std::endl;
}

void Enabled::react(ToDisable const &event) {
        transit<Disabled>();
}

void Enabled::react(ToIdle const &event) {
        transit<Idle>();
}