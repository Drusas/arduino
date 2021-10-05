#include "GaitController.h"
#include <iostream>

GaitController::GaitController(Configuration *config) {
    spotConfig = config;
}

GaitController::~GaitController() {}

void GaitController::getFootContacts(int ticks, uint8_t *modes) {
    // todo: check size
    modes = spotConfig->contactPhases[getPhaseIndex(ticks)];
}

int GaitController::getSubPhaseTicks(int ticks) {
    int phaseTime = ticks % spotConfig->phaseLength;
    int phaseSum = 0;
    int subPhaseTicks = 0;
    for (int i = 0; i < spotConfig->numPhases; i++) {
        phaseSum += spotConfig->phaseTicks[i];
        if (phaseTime < phaseSum) {
            subPhaseTicks = phaseTime - phaseSum + spotConfig->phaseTicks[i];
            return subPhaseTicks;
        }
    }
    return subPhaseTicks;
}

int GaitController::getPhaseIndex(int ticks) {
    std::cout << "getPhaseIndex" << std::endl;
    std::cout << "spotConfig->phaseLength: " << spotConfig->phaseLength << std::endl;
    std::cout << "spotConfig->numPhases: " << spotConfig->numPhases << std::endl;
    int phaseTime = ticks % spotConfig->phaseLength;
    std::cout << "phasetime: " << phaseTime << std::endl;
    int phaseSum = 0;
    for (int i = 0; i < spotConfig->numPhases; i++) {
        phaseSum += spotConfig->phaseTicks[i];
        std::cout << "getPhaseIndex: " << i << " phasesum:" << phaseSum << std::endl;
        if (phaseTime < phaseSum) {
            std::cout << "getPhaseIndex: " << i << std::endl;
            return i;
        }
    }
    std::cout << "getPhaseIndex: " << phaseSum << std::endl;
    return phaseSum;
}