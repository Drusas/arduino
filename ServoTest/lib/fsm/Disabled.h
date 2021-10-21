#ifndef _DISABLED_H
#define _DISABLED_H

#include "QuadrupedFsm.h"

class Disabled : public QuadrupedFsm {

public:
    void entry() override;
    void react(ToEnable const &event) override;
};

#endif