#include "Disabled.h"
#include <iostream>
#include "QuadrupedFsm.h"
#include "SpotFacade.h"
#include "tinyfsm.hpp"

Quadruped *SpotFacade::quadruped;
IServoController *SpotFacade::servoController;

Array2D *SpotFacade::LegPositions = &Quadruped::LegPositions;

void (*modeArray[4])();

int modeIdx = 0;

void SpotFacade::configure(Quadruped *q, IServoController *i) {
    SpotFacade::quadruped = q;
    SpotFacade::servoController = i;
    modeArray[0] = sit;
    modeArray[1] = lay;
    modeArray[2] = stand;
    modeArray[3] = walk;
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

void SpotFacade::updateLegAngles() {
    if (SpotFacade::quadruped == nullptr) {
        return;
    }

    SpotFacade::quadruped->updateLegAngles();
}

void SpotFacade::setLegAngles(uint8_t idx, float hx, float hy, float knee) {
    SpotFacade::quadruped->setLegAngles(idx, hx, hy, knee);
}

void SpotFacade::setLegPosition(uint8_t idx, float x, float y, float z) {
    SpotFacade::quadruped->setLegPosition(idx, x, y, z);
}

void SpotFacade::toggleMode() {
    if (getEnabled()) {
        int i = modeIdx++ % 4;
        (*modeArray[i])();
    }
}