#include "Disabled.h"
#include <iostream>
#include "QuadrupedFsm.h"
#include "SpotFacade.h"
#include "tinyfsm.hpp"

Quadruped *SpotFacade::quadruped;
IServoController *SpotFacade::servoController;

void SpotFacade::configure(Quadruped *q, IServoController *i) {
    SpotFacade::quadruped = q;
    SpotFacade::servoController = i;
}

void SpotFacade::setEnabled(bool state) {
    std::cout << "setEnabled(" << state << ")" << std::endl;
    if (state) {
        QuadrupedFsm::dispatch(ToEnable(SpotFacade::servoController));
    }
    else {
        QuadrupedFsm::dispatch(ToDisable(SpotFacade::servoController, SpotFacade::quadruped));
    }
}

 bool SpotFacade::getEnabled() {
    if (QuadrupedFsm::is_in_state<Disabled>()) {
        return false;
    }
    else {
        return true;
    }
}

void SpotFacade::sit() {
    QuadrupedFsm::dispatch(ToSit(SpotFacade::quadruped));
}

void SpotFacade::stand() {
    QuadrupedFsm::dispatch(ToStand(SpotFacade::quadruped));
}

void SpotFacade::lay() {
    QuadrupedFsm::dispatch(ToLay(SpotFacade::quadruped));
}

void SpotFacade::walk() {
    QuadrupedFsm::dispatch(ToWalk(SpotFacade::quadruped));
}