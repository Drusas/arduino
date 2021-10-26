#include "Disabled.h"
#include "Enabled.h"
#include "Idle.h"
#include <iostream>


void Enabled::entry() {
    std::cout << "state:enabled" << std::endl;
    transit<Idle>();
}

void Enabled::react(ToDisable const &event) {
	if (event.sController != nullptr) {
		event.sController->setEnabled(false);
		transit<Disabled>();
	}
}

void Enabled::react(ToIdle const &event) {
	transit<Idle>();
}