#ifndef _LEG_CONTROLLER_H
#define _LEG_CONTROLLER_H

#include "stdint.h"

class ILegController {
public:
    virtual void moveToXYZ(float x, float y, float z) = 0;
    virtual void moveToAngles(uint8_t hx, uint8_t hy, uint8_t k) = 0;
    virtual void setEnabled(bool state) = 0;
    virtual bool getEnabled() = 0;
    virtual void getXYZPosition(float *destination) = 0;
    virtual void getJointAngles(float *destination) = 0;
};

#endif