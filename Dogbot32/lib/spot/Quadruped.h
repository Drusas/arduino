#ifndef _QUADRUPED_H
#define _QUADRUPED_H

#include "Command.h"
#include "Controller.h"
#include "ILegController.h"
#include "ITask.h"

class Quadruped {
    Controller *controller;
    ILegController *legRF, *legLF, *legRR, *legLR;
    ITask *gaitTask;
    Command *cmd;

public:
    // buffer for transfering leg xyz and servo angles back and forth
    // RF -> [0,2], LF -> [3,5], RF -> [6,8], RF -> [9,11]
    static float LegPositions[3][4];    // buffer for transfering leg xyz and servo angles back and forth
    Quadruped();
    Quadruped(ILegController *RF, ILegController *LF, ILegController *RR, ILegController *LR);
    void configure(ITask *task, Controller *cntlr, Command *command, ILegController *RF, ILegController *LF, ILegController *RR, ILegController *LR);
    void sit();
    void stand();
    void lay();
    void walk();
    void stop();
    // void updateLegXYZPositions();
    // void updateLegJointAngles();
};

#endif