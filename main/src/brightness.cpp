#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "util.h"
#include "brightness.h"

namespace Brightness
{
    const uint8_t pinWhips = A2;
    const uint8_t pinAntennas = A3;

    void setup()
    {
        pinMode(pinWhips, INPUT);    // pot - whips
        pinMode(pinAntennas, INPUT); // pot - ant
    }

    // determine the current brightness settings 0-255
    void loop(uint8_t *brightnessWhips, uint8_t *brightnessAntennas)
    {
        *brightnessWhips = min(160, analogRead(pinWhips) / 4);
        *brightnessAntennas = min(160, analogRead(pinAntennas) / 4);
    }
}