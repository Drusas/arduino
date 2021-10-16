#include "GaitTask.h"
#include "Utils.h"

/*                                     RF   LF   RR   LR      */            
static float defaultStance[3][4] = {{   0,   0, -50, -50},
                                    {  60,  60,  60,  60},
                                    { 220, 220, 220, 220}};

void copyLocations(float src[][4], float dest[][4]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            dest[i][j] = src[i][j];
        }
    }
}

GaitTask::GaitTask(int interval, Controller *cntlr, State *ste, Command *cmd) {
    setEnabled(false);
    updateInterval = interval;
    controller = cntlr;
    state = ste;
    command = cmd;
    memset(newLocations, 0, 12*sizeof(float));
    memset(prevLocations, 0, 12*sizeof(float));
    memset(footContacts, 0, 4*sizeof(uint8_t));
    taskCounter = 0;
    LFLeg = nullptr;
}

void GaitTask::performUpdate() {
    if ((controller != nullptr)) { 
        controller->stepGait(state, command, newLocations, footContacts);
        copyLocations(newLocations, prevLocations);
        if (LFLeg != nullptr) {
            TRACE("GaitTask::PERFORMUPDATE, COUNTER=%d\n", taskCounter);
            printNewLocation();
            float x = defaultStance[0][0] + newLocations[0][0];
            float y = defaultStance[1][0] - newLocations[1][0];
            float z = defaultStance[2][0] - newLocations[2][0];
            RFLeg->addPoint(x, y, z);

            x = defaultStance[0][1] + newLocations[0][1];
            y = defaultStance[1][1] - newLocations[1][1];
            z = defaultStance[2][1] - newLocations[2][1];
            LFLeg->addPoint(x, y, z);

            x = defaultStance[0][2] + newLocations[0][2];
            y = defaultStance[1][2] - newLocations[1][2];
            z = defaultStance[2][2] - newLocations[2][2];
            RRLeg->addPoint(x, y, z);

            x = defaultStance[0][3] + newLocations[0][3];
            y = defaultStance[1][3] - newLocations[1][3];
            z = defaultStance[2][3] - newLocations[2][3];
            LRLeg->addPoint(x, y, z);
        }
        if (taskCounter % 10 == 0) {
        }
    }
    taskCounter++;
}

void GaitTask::printNewLocation() {
    TRACE("X: %5.2f, %5.2f, %5.2f, %5.2f\n", defaultStance[0][0] - (1 * newLocations[0][0]), defaultStance[0][1] - (1 * newLocations[0][1]), defaultStance[0][2] - (1 * newLocations[0][2]), defaultStance[0][3] - (1 * newLocations[0][3]));
    TRACE("Y: %5.2f, %5.2f, %5.2f, %5.2f\n", defaultStance[1][0] - (1 * newLocations[1][0]), defaultStance[1][1] - (1 * newLocations[1][1]), defaultStance[1][3] - (1 * newLocations[1][2]), defaultStance[1][3] - (1 * newLocations[1][3]));
    TRACE("Z: %5.2f, %5.2f, %5.2f, %5.2f\n", defaultStance[2][0] - (1 * newLocations[2][0]), defaultStance[2][1] - (1 * newLocations[2][1]), defaultStance[2][3] - (1 * newLocations[2][2]), defaultStance[2][3] - (1 * newLocations[2][3]));
}

void GaitTask::setEnabled(bool state) {
    TRACE("GaitTask::setEnabled, COUNTER=%d\n", taskCounter);
    if (state) {
        taskCounter = 0;
    }
    ITask::setEnabled(state);
}

void GaitTask::setLegs(LegController *RF, LegController *LF, LegController *RR, LegController *LR) {
    RFLeg = RF;
    LFLeg = LF;
    RRLeg = RR;
    LRLeg = LR;
}