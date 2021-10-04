#include "GaitController.h"

GaitController::GaitController(Configuration *config) {
    spotConfig = config;
}

GaitController::~GaitController() {}

void GaitController::getFootContacts(long ticks, uint8_t *modes) {
    // todo: check size
    modes = spotConfig->contactPhases[getPhaseIndex(ticks)];
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
    int phaseTime = ticks % spotConfig->phaseLength;
    int phaseSum = 0;
    for (int i = 0; i < spotConfig->numPhases; i++) {
        phaseSum += spotConfig->phaseTicks[i];
        if (phaseTime < phaseSum) {
            return i;
        }
    }
    return phaseSum;
}