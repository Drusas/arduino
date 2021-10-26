#include "GaitTask.h"
#include "Utils.h"

/*                                     RF   LF   RR   LR      */            
static float gaitDefaultStance[3][4] = {{  25,  25, -25, -25},
                                        {  60,  60,  60,  60},
                                        { 200, 200, 200, 200}};

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
    TRACE("GaitTask::PERFORMUPDATE, COUNTER=%d\n", taskCounter);
    if ((controller != nullptr)) {
        taskCounter++;
        if (areAnyLegBuffersFull()) {
            TRACE("GaitTask::performUpdate a leg buffer is full() %d %d %d %d\n", RFLeg->isPositionBufferFull(), LFLeg->isPositionBufferFull(), RRLeg->isPositionBufferFull(), LRLeg->isPositionBufferFull());
            return;
        } 
        controller->stepGait(state, command, newLocations, footContacts);
        printNewLocation();
        copyLocations(newLocations, prevLocations);
        if (LFLeg != nullptr) {
            float x = gaitDefaultStance[0][0] + newLocations[0][0];
            float y = gaitDefaultStance[1][0] - newLocations[1][0];
            float z = gaitDefaultStance[2][0] - newLocations[2][0];
            RFLeg->addPoint(x, y, z);

            x = gaitDefaultStance[0][1] + newLocations[0][1];
            y = gaitDefaultStance[1][1] - newLocations[1][1];
            z = gaitDefaultStance[2][1] - newLocations[2][1];
            LFLeg->addPoint(x, y, z);

            x = gaitDefaultStance[0][2] + newLocations[0][2];
            y = gaitDefaultStance[1][2] - newLocations[1][2];
            z = gaitDefaultStance[2][2] - newLocations[2][2];
            RRLeg->addPoint(x, y, z);

            x = gaitDefaultStance[0][3] + newLocations[0][3];
            y = gaitDefaultStance[1][3] - newLocations[1][3];
            z = gaitDefaultStance[2][3] - newLocations[2][3];
            LRLeg->addPoint(x, y, z);
        }
    }
}

void GaitTask::printNewLocation() {
    TRACE("X: %5.2f, %5.2f, %5.2f, %5.2f\n", gaitDefaultStance[0][0] - (1 * newLocations[0][0]), gaitDefaultStance[0][1] - (1 * newLocations[0][1]), gaitDefaultStance[0][2] - (1 * newLocations[0][2]), gaitDefaultStance[0][3] - (1 * newLocations[0][3]));
    TRACE("Y: %5.2f, %5.2f, %5.2f, %5.2f\n", gaitDefaultStance[1][0] - (1 * newLocations[1][0]), gaitDefaultStance[1][1] - (1 * newLocations[1][1]), gaitDefaultStance[1][3] - (1 * newLocations[1][2]), gaitDefaultStance[1][3] - (1 * newLocations[1][3]));
    TRACE("Z: %5.2f, %5.2f, %5.2f, %5.2f\n", gaitDefaultStance[2][0] - (1 * newLocations[2][0]), gaitDefaultStance[2][1] - (1 * newLocations[2][1]), gaitDefaultStance[2][3] - (1 * newLocations[2][2]), gaitDefaultStance[2][3] - (1 * newLocations[2][3]));
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

bool GaitTask::areAnyLegBuffersFull() {
    if (RFLeg == nullptr || LFLeg == nullptr || RRLeg == nullptr || LRLeg == nullptr) {
        return true;
    }
    bool result = false;
    result = RFLeg->isPositionBufferFull() || LFLeg->isPositionBufferFull() || RRLeg->isPositionBufferFull() || LRLeg->isPositionBufferFull();
    //printf("%d %d %d %d\n", RFLeg->isPositionBufferFull(), LFLeg->isPositionBufferFull(), RRLeg->isPositionBufferFull(), LRLeg->isPositionBufferFull());
    return result;
}