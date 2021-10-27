#ifndef _LEGCONTROLLER_H
#define _LEGCONTROLLER_H

#include <Adafruit_PWMServoDriver.h>
#include <Arduino.h>
#include "CircularBuffer.h"
#include "IKModel.h"
#include "ILegController.h"
#include "IMotor.h"
#include "ArdTask.h"
#include "IServoController.h"
#include <Wire.h>

#define DEBUG_LEGCONTROLLER 0

struct LegPosition {
    uint8_t capsule;
    uint8_t shoulder;
    uint8_t knee;
};

class LegController : public ILegController, public ArdTask
{
    static const size_t NUM_POSITIONS = 16;
    IServoController *servoController;
    IMotor *hipxMotor; // rotates about the x-axis (y-z plane)
    IMotor *hipyMotor; // rotates about the y-axis (x-z plane)
    IMotor *kneeMotor;
    
    Point points[NUM_POSITIONS];
    CircularBuffer<Point> positionBuffer = CircularBuffer<Point>(points, NUM_POSITIONS);
    uint8_t posIdx, bufferIdx;
    LegIKModel ikModel;
  
    bool areAllMotorsAtPosition();

protected:
    void performUpdate();
    
 public:
    LegController();
    LegController(int interval, float femurLength, float tibiaLength, float zOffset, float yOffset, IMotor *hipx, IMotor *hipy, IMotor *knee, IServoController *controller);
    void configure(int interval, float femurLength, float tibiaLength, float zOffset, float yOffset, IMotor *hipx, IMotor *hipy, IMotor *knee, IServoController *controller);
    bool isPositionBufferFull();
    void addPoint(Point p);
    void addPoint(float x, float y, float z);
    void moveToXYZ(float x, float y, float z) override;
    void moveToAngles(uint8_t hx, uint8_t hy, uint8_t k);
    IMotor* getJoint(uint8_t idx);
    void setHxTranslationAndOffset(float translate, float offset, int sign);
    void setHyTranslationAndOffset(float translate, float offset, int sign);
    void setKneeTranslationAndOffset(float translate, float offset, int sign);
    void setHxConstraints(float min, float max);
    void setHyConstraints(float min, float max);
    void setKneeConstraints(float min, float max);
    void followTrajectory(Point *buffer, uint8_t numPoints);
    void setEnabled(bool state) override;
    bool getEnabled() override;
};

#endif
