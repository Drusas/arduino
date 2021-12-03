#ifndef _QUADRUPED_H
#define _QUADRUPED_H

#include "Command.h"
#include "Controller.h"
#include "ILegController.h"
#include "ITask.h"

#define A2D_ROW 3
#define A2D_COL 4

typedef float Array2D[A2D_ROW][A2D_COL]; 

class Quadruped {
    Controller *controller;
    ILegController *legRF, *legLF, *legRR, *legLR;
    ITask *gaitTask;
    Command *cmd;

public:
    // buffer for transfering leg xyz and servo angles back and forth
    // RF -> [0,2], LF -> [3,5], RF -> [6,8], RF -> [9,11]
    static Array2D LegPositions;    // buffer for transfering leg xyz and servo angles back and forth
    Quadruped();
    Quadruped(ILegController *RF, ILegController *LF, ILegController *RR, ILegController *LR);
    void configure(ITask *task, Controller *cntlr, Command *command, ILegController *RF, ILegController *LF, ILegController *RR, ILegController *LR);
    void sit();
    void stand();
    void lay();
    void walk();
    void stop();
    void updateLegXYZPositions();
    void updateLegAngles();
    void setLegAngles(uint8_t idx, float hx, float hy, float knee);
    void setLegPosition(uint8_t idx, float x, float y, float z);
    void setWalkingSpeed(float xVelocity, float yVelocity);
    void setBodySpeed(float xVelocity, float yVelocity);
};

#endif