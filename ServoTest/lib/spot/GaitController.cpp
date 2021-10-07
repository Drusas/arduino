#include "GaitController.h"
#include <iostream>

GaitController::GaitController(Configuration *config) {
    spotConfig = config;
}

GaitController::~GaitController() {}

void GaitController::getFootContacts(long ticks, uint8_t *modes) {
    // todo: check size
    int idx = getPhaseIndex(ticks);
    for (int i = 0; i < 4; i++) {
        modes[i] = spotConfig->contactPhases[i][idx];
    }
    // modes = spotConfig->contactPhases[getPhaseIndex(ticks)];
}

int GaitController::getSubPhaseTicks(long ticks) {
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

int GaitController::getPhaseIndex(long ticks) {
    // std::cout << "getPhaseIndex: " << ticks << std::endl;
    // std::cout << "spotConfig->phaseLength: " << spotConfig->phaseLength << std::endl;
    // std::cout << "spotConfig->numPhases: " << spotConfig->numPhases << std::endl;
    int phaseTime = 0; 
    phaseTime = ticks % spotConfig->phaseLength;
    // std::cout << "phasetime: " << phaseTime << std::endl;
    int phaseSum = 0;
    for (int i = 0; i < spotConfig->numPhases; i++) {
        phaseSum += spotConfig->phaseTicks[i];
        // std::cout << "currentIndex: " << i << " phasesum:" << phaseSum << std::endl;
        if (phaseTime < phaseSum) {
            // std::cout << "returnedPhaseIndex: " << i << std::endl;
            return i;
        }
    }
    // std::cout << "returnedPhaseIndex: " << phaseSum << std::endl;
    return phaseSum;
}