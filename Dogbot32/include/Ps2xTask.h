#ifndef _PS2X_TASK_H
#define _PS2X_TASK_H

#include "ArdTask.h"
#include "SpotFacade.h"

class Ps2xTask : public ArdTask {
    SpotFacade *spot;

public:
    Ps2xTask();
    void configure(int interval, SpotFacade *sf);

protected:
    void performUpdate() override;
};

#endif