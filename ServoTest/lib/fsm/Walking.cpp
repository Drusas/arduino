#include "Disabled.h"
#include <iostream>
#include "Standing.h"
#include "Walking.h"

void Walking::entry() {
    std::cout << "state:walking" << std::endl;
}

void Walking::react(ToStand const &event) {
        transit<Standing>();
}

void Walking::react(ToDisable const &event) {
        transit<Disabled>();
}