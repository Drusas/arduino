#include <Arduino.h>
#include "Utils.h"


float round_up(float value, uint8_t decimal_places) {
    const float multiplier = std::pow(10.0, decimal_places);
    return std::ceil(value * multiplier) / multiplier;
}