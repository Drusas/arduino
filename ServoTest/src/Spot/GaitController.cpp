#include "GaitController.h"

GaitController::GaitController(Configuration *config) {
    spotConfig = config;
}

GaitController::~GaitController() {}

// void GaitController::getFootContacts(long ticks, uint8_t *modes) {
//     // todo: check size
//     modes = spotConfig->contactPhases[getPhaseIndex(ticks)];
// }

// int GaitController::getPhaseIndex(long ticks) {
//     int phaseTime = ticks % config->phaseLength;
//     phase_time = ticks % self.config.phase_length
//         phase_sum = 0
//         subphase_ticks = 0
//         for i in range(self.config.num_phases):
//             phase_sum += self.config.phase_ticks[i]
//             if phase_time < phase_sum:
//                 subphase_ticks = phase_time - phase_sum + self.config.phase_ticks[i]
//                 return subphase_ticks
// }