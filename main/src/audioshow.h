#pragma once
#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

namespace Audioshow
{
    void setup();
    void loop();
    bool jack_plugged_in();

    extern float leftPeak;
    extern float rightPeak;
    extern float fftBands[];
}; // namespace Audioshow