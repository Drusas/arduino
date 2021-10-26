#ifndef _QUADRUPED_H
#define _QUADRUPED_H

#include "Command.h"
#include "ILegController.h"
#include "ITask.h"

class Quadruped {
    ILegController *legRF, *legLF, *legRR, *legLR;
    ITask *gaitTask;
    Command *cmd;

public:
    Quadruped();
    Quadruped(ILegController *RF, ILegController *LF, ILegController *RR, ILegController *LR);
    void setLegs(ILegController *RF, ILegController *LF, ILegController *RR, ILegController *LR);
    void setWalkParameters(ITask *task, Command *command);
    void stand();
    void walk();
    void stop();
};

#endif