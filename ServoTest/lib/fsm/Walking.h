#ifndef _WALKING_H
#define _WALKING_H

#include "QuadrupedFsm.h"

class Walking : public QuadrupedFsm {

public:
    void entry() override;
    void react(ToStand const &event) override;
};

#endif