#pragma once
#define FASTLED_INTERNAL
#include <FastLED.h>

namespace Led
{
    void setup();
    void loop(
        Controller::ButtonState *pButtonState,
        uint8_t brightnessWhips,
        uint8_t brightnessAntennas,
        uint8_t leftPeak,
        uint8_t rightPeak);

}; // namespace Led