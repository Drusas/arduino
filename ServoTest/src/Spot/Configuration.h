#ifndef _SPOT_CONFIGURATION_H
#define _SPOT_CONFIGURATION_H

#include <Arduino.h>

class Configuration {

public:
    // there are 8 contact phases when walking
    // we'll access each phase as a column in the array.
    uint8_t contactPhases[4][8] = {{1, 0, 1, 1, 1, 1, 1, 1}, 
                                   {1, 1, 1, 0, 1, 1, 1, 1},
                                   {1, 1, 1, 1, 1, 0, 1, 1},
                                   {1, 1, 1, 1, 1, 1, 1, 0}};
    float deltaX = 0.1;
    float deltaY = 0.09;
    float xShift = 0.0;
    float defaultZRef = -0.16;                             
    float defaultStance[3][4] = {{deltaX + xShift, deltaX + xShift, -deltaX + xShift, deltaX + xShift},
                                 {-deltaY, deltaY, -deltaY, deltaY},
                                 {0, 0, 0, 0}};
    int phaseLength;
    int numPhases = 8;
    float overlapTime = 0.1;
    float dt = 0.01;
    float swingTime = 0.15;
    int swingTicks = (int)(swingTime / dt);
    int overlapTicks = (int)(overlapTime / dt);
    int phaseTicks[8] = { overlapTicks, swingTicks, overlapTicks, swingTicks,
                       overlapTicks, swingTicks, overlapTicks, swingTicks };
    float zTimeConstant = 0.02;
    float zClearance = 7.0;
    float alpha = 0.5;
    int stanceTicks = 2 * overlapTicks + swingTicks;

    Configuration();
    ~Configuration();
    void getDefaultStanceLocaton(int legIdx, float* location);
};

#endif