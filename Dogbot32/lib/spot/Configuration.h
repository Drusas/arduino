#ifndef _SPOT_CONFIGURATION_H
#define _SPOT_CONFIGURATION_H

// #include <Arduino.h>
#include <stdint.h>

class Configuration {

public:
    // MOVEMENT PARAMS
    float zTimeConstant = 0.02;

    // STANCE
    float deltaX = 0.1;
    float deltaY = 0.09;
    float xShift = 0.0;
    float defaultZRef = -0.16;

    // SWING
    float zClearance = 50.0;
    float alpha = 0.5;

    // GAIT
    float dt = 0.01;
    int numPhases = 8;
    unsigned char contactPhases[4][8] = {{1, 0, 1, 1, 1, 1, 1, 1}, // there are 8 contact phases when walking we'll access each phase as a column in the array.
                                         {1, 1, 1, 0, 1, 1, 1, 1},
                                         {1, 1, 1, 1, 1, 0, 1, 1},
                                         {1, 1, 1, 1, 1, 1, 1, 0}};
    float overlapTime = 0.1;
    float swingTime = 0.15;
    int overlapTicks = (int)(overlapTime / dt);
    int swingTicks = (int)(swingTime / dt);
    int stanceTicks = 2 * overlapTicks + swingTicks;
    int phaseTicks[8] = { overlapTicks, swingTicks, overlapTicks, swingTicks,
                          overlapTicks, swingTicks, overlapTicks, swingTicks };
    int phaseLength =  2 * (2 * overlapTicks + 2 * swingTicks);

    // DEFAULT STANCE
                                    /*     RF              LF                 RR                LR      */                          
    float defaultStance[3][4] = {{deltaX + xShift, deltaX + xShift, -deltaX + xShift, -deltaX + xShift},
                                 {-deltaY,         deltaY,          -deltaY,          deltaY         },
                                 { 0,              0,                0,               0              }};

    // float defaultStance[3][4] = {{  24,  25, -25, -25},
    //                                 {  60,  60,  60,  60},
    //                                 { 200, 200, 200, 200}};
    
    Configuration();
    ~Configuration() {};
    void getDefaultStanceLocaton(int legIdx, float* location);
};

#endif