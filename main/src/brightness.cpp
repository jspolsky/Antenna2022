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
    void loop(uint8_t *pbrightnessWhips, uint8_t *pbrightnessAntennas)
    {
        *pbrightnessWhips = max(8, min(160, analogRead(pinWhips) / 4));
        *pbrightnessAntennas = max(8, min(160, analogRead(pinAntennas) / 4));
    }
}