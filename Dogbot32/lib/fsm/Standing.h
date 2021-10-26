#ifndef _STANDING_H
#define _STANDING_H

#include "QuadrupedFsm.h"

class Standing : public QuadrupedFsm {

public:
    void entry() override;
    void react(ToIdle const &event) override;
    void react(ToWalk const &event) override;
    void react(ToDisable const &event) override;
};

#endif