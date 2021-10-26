#ifndef _SITTING_H
#define _SITTING_H

#include "QuadrupedFsm.h"

class Sitting : public QuadrupedFsm {

public:
    void entry() override;
    void react(ToIdle const &event) override;
    void react(ToStand const &event) override;
    void react(ToDisable const &event) override;
};

#endif