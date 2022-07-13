
#include <Arduino.h>
#include <OctoWS2811.h>
#include "Teensy4Controller.h"
#include "Util.h"
#include "led.h"

namespace Led
{

  //
  // Physical arrangement of LEDs
  //

  const int numPins = 24;
  byte pinList[numPins] = {
      2, 9, 24,   // ANT - North - Bottom to Top
      27, 28, 31, // ANT - East - Bottom to Top
      3, 4, 25,   // ANT - South - Bottom to Top
      26, 29, 30, // ANT - West - Bottom to Top

      32, 35, 36, 39, 40, 22, // WHIPS - Left - Center out
      33, 34, 37, 38, 41, 14, // WHIPS - Right - Center out
  };

  const int ledsPerStrip = 300;
  CRGB pixels[numPins * ledsPerStrip];

  // These buffers need to be large enough for all the pixels.
  // The total number of pixels is "ledsPerStrip * numPins".
  // Each pixel needs 3 bytes, so multiply by 3.  An "int" is
  // 4 bytes, so divide by 4.  The array is created using "int"
  // so the compiler will align it to 32 bit memory.
  DMAMEM int displayMemory[ledsPerStrip * numPins * 3 / 4];
  int drawingMemory[ledsPerStrip * numPins * 3 / 4];
  OctoWS2811 octo(ledsPerStrip, displayMemory, drawingMemory, WS2811_RGB | WS2811_800kHz, numPins, pinList);
  CTeensy4Controller<RGB, WS2811_800kHz> *pcontroller;

  void setup()
  {
    octo.begin();
    pcontroller = new CTeensy4Controller<RGB, WS2811_800kHz>(&octo);

    FastLED.setBrightness(160);

    // FastLED.setCorrection(TypicalLEDStrip);
    // FastLED.setTemperature(DirectSunlight);

    FastLED.addLeds(pcontroller, pixels, numPins * ledsPerStrip);
  }

  void setPixelColor(int ixStrip, int ixPosition, CRGB rgb)
  {
    pixels[ixStrip * ledsPerStrip + ixPosition] = rgb;
  }

  static uint8_t test_color = 0;

  void testPattern(uint8_t leftPeak, uint8_t rightPeak)
  {
    // Antenna tests
    for (int ixStrip = 0; ixStrip <= 11; ixStrip++)
    {
      // Set background colors to indicate direction
      for (int ixPosition = 0; ixPosition < 300; ixPosition++)
      {
        CRGB rgb = CRGB::Black;

        switch (ixStrip)
        {
        case 0:
        case 1:
        case 2:
          rgb = CRGB::Red;
          break; // RED NORTH

        case 3:
        case 4:
        case 5:
          rgb = CRGB::Green;
          break; // GREEN EAST

        case 6:
        case 7:
        case 8:
          rgb = CRGB::Blue;
          break; // BLUE SOUTH

        default:
          rgb = CRGB(0x65, 0x43, 0x21);
          break; // ORANGE WEST
        }

        // ALTERNATELY - test strips by going R, G, B
        EVERY_N_SECONDS(1)
        {
          test_color = (test_color + 1) % 3;
        }
        switch (test_color)
        {
        case 0:
          rgb = CRGB::Red;
          break;

        case 1:
          rgb = CRGB::Green;
          break;

        default:
          rgb = CRGB::Blue;
          break;
        }

        setPixelColor(ixStrip, ixPosition, rgb);
      }

      // indicate strip number by turning off some LEDs:
      // for (int ixPosition = 0; ixPosition < 300; ixPosition += (ixStrip + 2))
      //  setPixelColor(ixStrip, ixPosition, CRGB::Black);
    }

    // Whip tests
    for (int ixStrip = 0; ixStrip < 12; ixStrip++)
    {
      // left channel audio
      if (ixStrip < 6)
      {
        uint8_t leftPeak_inv = 6 - leftPeak;
        if (ixStrip < leftPeak_inv)
        {
          for (int ixPosition = 0; ixPosition < 110; ixPosition++)
            setPixelColor(ixStrip + 12, ixPosition, CRGB::Black);
        }
        else
        {
          for (int ixPosition = 0; ixPosition < 110; ixPosition++)
            setPixelColor(ixStrip + 12, ixPosition, CRGB(CHSV(((ixStrip - 12) * 20), 255, 255)));
        }
      }
      // right channel
      else
      {
        if (ixStrip - 5 > rightPeak)
        {
          for (int ixPosition = 0; ixPosition < 110; ixPosition++)
            setPixelColor(ixStrip + 12, ixPosition, CRGB::Black);
        }
        else
        {
          for (int ixPosition = 0; ixPosition < 110; ixPosition++)
            setPixelColor(ixStrip + 12, ixPosition, CRGB(CHSV(((ixStrip - 12) * 20), 255, 255)));
        }
      }
    }

    FastLED.show();
  }

  void loop(uint8_t leftPeak, uint8_t rightPeak)
  {
    testPattern(leftPeak, rightPeak);
  }
};
