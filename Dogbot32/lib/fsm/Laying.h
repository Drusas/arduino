#ifndef _LAYING_H
#define _LAYING_H

#include "QuadrupedFsm.h"

class Laying : public QuadrupedFsm {

public:
    void entry() override;
    void react(ToIdle const &event) override;
    void react(ToStand const &event) override;
    void react(ToSit const &event) override;
    void react(ToDisable const &event) override;
};

#endif