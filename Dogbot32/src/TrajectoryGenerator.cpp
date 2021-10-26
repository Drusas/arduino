#include "TrajectoryGenerator.h"

Point* TrajectoryGenerator::GenerateCircle(float originX, float originY, float radius, uint8_t numPoints) {
    Point* points = new Point[numPoints];
    float angle = (2 * PI) / numPoints;
    for (uint8_t i = 0; i < numPoints; i++) {
        points[i].x = radius * cos(angle * i) + originX;
        points[i].y = 60;
        points[i].z = radius * sin(angle * i) + originY;
    }
    return points;
}
Point* TrajectoryGenerator::GenerateEllipse(float originX, float originY, float r1, float r2, uint8_t numPoints) {
    Point* points = new Point[numPoints];
    float angle = (2 * PI) / numPoints;
    for (uint8_t i; i < numPoints; i++) {
        points[i].x = r1 * cos(angle * i) + originX;
        points[i].y = 60;
        points[i].z = r2 * sin(angle * i) + originY;
    }
    return points;
}

Point* TrajectoryGenerator::GenerateRectangle(float originX, float originY, float h, float w, uint8_t numPoints) {
    Point* points = new Point[4];
    points[0].x = originX + (w / 2);
    points[0].y = 60;
    points[0].z = originY - (h / 2);

    points[1].x = originX + (w / 2);
    points[1].y = 60;
    points[1].z = originY + (h / 2);

    points[2].x = originX - (w / 2);
    points[2].y = 60;
    points[2].z = originY + (h / 2);

    points[3].x = originX - (w / 2);
    points[3].y = 60;
    points[3].z = originY - (h / 2);

    return points;
}