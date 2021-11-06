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
    Quadruped();
    Quadruped(ILegController *RF, ILegController *LF, ILegController *RR, ILegController *LR);
    void configure(ITask *task, Controller *cntlr, Command *command, ILegController *RF, ILegController *LF, ILegController *RR, ILegController *LR);
    void sit();
    void stand();
    void lay();
    void walk();
    void stop();
};

#endif