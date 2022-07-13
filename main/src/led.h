#pragma once
#define FASTLED_INTERNAL
#include <FastLED.h>

namespace Led
{
    void setup();
    void loop(uint8_t leftPeak, uint8_t rightPeak);

}; // namespace Led