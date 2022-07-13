#pragma once
#include <Arduino.h>

namespace Audioshow
{
    void setup();
    void loop();

    extern uint8_t leftPeak;
    extern uint8_t rightPeak;
}; // namespace Audioshow