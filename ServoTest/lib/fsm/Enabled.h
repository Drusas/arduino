#ifndef _ENABLED_H
#define _ENABLED_H

#include "QuadrupedFsm.h"

class Enabled : public QuadrupedFsm {

public:
    void entry() override;
    void react(Disable const &event) override;
    void react(ToIdle const &event) override;
};

#endif