#include "Disabled.h"
#include "Enabled.h"
#include <iostream>


void Disabled::entry() {
    std::cout << "state:disabled" << std::endl;
}

void Disabled::react(ToEnable const &event) {
    if (event.sController != nullptr) {
        event.sController->setEnabled(true);
        transit<Enabled>();
    }
}