#ifndef _SPOT_FACADE_H
#define _SPOT_FACADE_H

#include "IServoController.h"
#include "Quadruped.h"

class SpotFacade {

    static Quadruped *quadruped;
    static IServoController *servoController;

public:
    static Array2D *LegPositions;    // buffer for transfering leg xyz and servo angles back and forth
    static void configure(Quadruped *q, IServoController *i);
    static void setEnabled(bool state);
    static bool getEnabled();
    static void sit();
    static void stand();
    static void lay();
    static void walk();
    void updateLegAngles();
    void setLegAngles(uint8_t idx, float hx, float hy, float knee);
    void setLegPosition(uint8_t idx, float x, float y, float z);
    void toggleMode();
};
#endif
