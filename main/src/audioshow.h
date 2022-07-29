#pragma once
#include <Arduino.h>

namespace Audioshow
{
    void setup();
    void loop();
    bool jack_plugged_in();

    extern float leftPeak;
    extern float rightPeak;
}; // namespace Audioshow