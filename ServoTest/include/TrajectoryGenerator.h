#ifndef _TRAJECTORY_GENERATOR_H
#define _TRAJECTORY_GENERATOR_H

#include <Arduino.h>
#include "Utils.h"

class TrajectoryGenerator {

public:
    static Point* GenerateCircle(float originX, float originY, float radius, uint8_t numPoints);
    static Point* GenerateEllipse(float originX, float originY, float r1, float r2, uint8_t numPoints);
    static Point* GenerateRectangle(float originX, float originY, float h, float w, uint8_t numPoints);
};

#endif