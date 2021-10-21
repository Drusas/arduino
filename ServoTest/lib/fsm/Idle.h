#ifndef _IDLE_H
#define _IDLE_H

#include "QuadrupedFsm.h"

class Idle : public QuadrupedFsm {

public:
    void entry() override;
    void react(ToStand const &event) override;
    void react(ToDisable const &event) override;
};

#endif