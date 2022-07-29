#pragma once
#include <Arduino.h>

namespace Audioshow
{
    void setup();
    void loop();
    bool jack_plugged_in();

    extern uint8_t leftPeak;
    extern uint8_t rightPeak;
}; // namespace Audioshow