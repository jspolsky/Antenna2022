#pragma once

#define FASTLED_INTERNAL
#include <FastLED.h>

namespace Controller
{
    typedef struct tagButtonState
    {
        uint8_t cColors;       // the number of solid colors to display, 0 if running an animation
        uint8_t rgbuttons[8];  // which 8 buttons are down
        CRGB rgrgb[8];         // which RGB colors to show (based on the order they were pressed), only 0..cColors-1 are populated
        uint8_t animation;     // which animation is showing (0-7) 
    } ButtonState;

    void setup();
    ButtonState *loop();
}
