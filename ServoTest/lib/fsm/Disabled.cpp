#include "Disabled.h"
#include "Enabled.h"
#include <iostream>


void Disabled::entry() {
    std::cout << "state:disabled" << std::endl;
}

void Disabled::react(Enable const &event) {
        transit<Enabled>();
}