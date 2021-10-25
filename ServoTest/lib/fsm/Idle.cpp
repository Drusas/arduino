#include "Disabled.h"
#include "Idle.h"
#include <iostream>
#include "Standing.h"

void Idle::entry() {
    std::cout << "state:idle" << std::endl;
}

void Idle::react(ToStand const &event) {
        
        transit<Standing>();
}

void Idle::react(ToDisable const &event) {
        transit<Disabled>();
}