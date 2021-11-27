#ifndef _SPOT_FACADE_H
#define _SPOT_FACADE_H

#include "IServoController.h"
#include "Quadruped.h"

class SpotFacade {

    static Quadruped *quadruped;
    static IServoController *servoController;

public:
    static void configure(Quadruped *q, IServoController *i);
    static void setEnabled(bool state);
    static bool getEnabled();
    static void sit();
    static void stand();
    static void lay();
    static void walk();
};
#endif
