#ifndef _WALKING_H
#define _WALKING_H

#include "Quadruped.h"
#include "QuadrupedFsm.h"

class Walking : public QuadrupedFsm {
    void stopWalking(Quadruped *q);
public:
    void entry() override;
    void exit() override;
    void react(ToStand const &event) override;
    void react(ToDisable const &event) override;
};

#endif