
#include <Arduino.h>
#include <OctoWS2811.h>
#include "Teensy4Controller.h"
#include "Util.h"
#include "controller.h"
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

      32, 35, 36, 39, 40, 22, // WHIPS - Left - Right
      33, 34, 37, 38, 41, 14, // WHIPS continued
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

  uint8_t g_brightnessWhips = 0, g_brightnessAntennas = 0;
  const int redBlinky = 891; // where the red blinking light for the top starts

  void setup()
  {
    octo.begin();
    pcontroller = new CTeensy4Controller<RGB, WS2811_800kHz>(&octo);
    FastLED.setBrightness(255); // we do our own dimming

    // FastLED.setCorrection(TypicalLEDStrip);
    // FastLED.setTemperature(DirectSunlight);
    FastLED.setDither(0);
    FastLED.addLeds(pcontroller, pixels, numPins * ledsPerStrip);
  }

  //
  // USEFUL PIXEL-MAPPING UTILITY FUNCTIONS
  //

  void setPixelColor(int ixStrip, int ixPosition, CRGB rgb, uint8_t brightness)
  {
    pixels[ixStrip * ledsPerStrip + ixPosition] = rgb % brightness;
  }

  // set a pixel on the antenna; 0 is the bottom, 899 is the top.
  // Specify a direction 0 - 3 or leave out for all sides
  void setAntennaPixel(uint16_t pixel, CRGB rgb, int oneDirection = 4)
  {
    for (int direction = 0; direction < 4; direction++)
    {
      if (oneDirection == 4 || oneDirection == direction)
        setPixelColor(pixel / 300 + 3 * direction, pixel % 300, rgb, pixel >= redBlinky ? 255 : g_brightnessAntennas);
    }
  }

  // set a pixel on the whips; 0 is the bottom, 110 is the top.
  // Specify a whip 0 - 11 or leave out for all whips
  void setWhipPixel(uint16_t pixel, CRGB rgb, int oneWhip = 12)
  {
    for (int whip = 0; whip < 12; whip++)
    {
      if (oneWhip == 12 || oneWhip == whip)
        setPixelColor(whip + 12, pixel, rgb, g_brightnessWhips);
    }
  }

  void antennaTestPattern()
  {
    static uint8_t test_hue = 0;

    EVERY_N_MILLISECONDS(10)
    {
      test_hue = (test_hue + 1) % 255;
    }

    uint16_t height = (millis() % 1000) * 9 / 10;

    for (uint16_t i = 0; i < 900; i++)
    {
      if (i > height)
        setAntennaPixel(i, CRGB::Black);
      else
        setAntennaPixel(i, CHSV(test_hue, 255, 255));
    }
  }

  void antennaSolidColor(CRGB rgb)
  {
    for (uint16_t i = 0; i < 900; i++)
    {
      setAntennaPixel(i, rgb);
    }
  }

  void whipSolidColor(CRGB rgb, int oneWhip = 12)
  {
    for (uint16_t i = 0; i < 110; i++)
    {
      setWhipPixel(i, rgb, oneWhip);
    }
  }

  // Some solid color buttons are being pressed; show those colors
  //
  void solidPattern(Controller::ButtonState *pbuttonState)
  {
    // the ANTENNA always goes solid with the first color
    antennaSolidColor(pbuttonState->rgrgb[0]);

    // only 1 color? The whips will join it!
    if (pbuttonState->cColors == 1)
    {
      whipSolidColor(pbuttonState->rgrgb[0]);
    }
    else
    {
      int whichColor = 1;

      // otherwise the whips get divided up among the remaining colors
      for (int oneWhip = 0; oneWhip < 12; oneWhip++)
      {
        whipSolidColor(pbuttonState->rgrgb[whichColor], oneWhip);
        whichColor++;
        if (whichColor == pbuttonState->cColors)
          whichColor = 1;
      }
    }
  }

  void whipTestPattern()
  {
    static uint8_t test_hue = 0;
    static uint8_t test_whip = 0;

    EVERY_N_MILLISECONDS(100)
    {
      test_hue = (test_hue + 1) % 255;
    }

    EVERY_N_SECONDS(1)
    {
      test_whip = (test_whip + 1) % 13;
    }

    uint16_t height = (millis() % 1000) * 110 / 1000;

    for (uint16_t i = 0; i < 110; i++)
    {
      if (i > height)
        setWhipPixel(i, CRGB::Black, test_whip);
      else
        setWhipPixel(i, CHSV(test_hue, 255, 255), test_whip);
    }
  }

  // void whipTestPatternAudio(uint8_t leftPeak, uint8_t rightPeak)
  // {

  //   // Whip tests
  //   for (int ixStrip = 0; ixStrip < 12; ixStrip++)
  //   {
  //     // left channel audio
  //     if (ixStrip < 6)
  //     {
  //       uint8_t leftPeak_inv = 6 - leftPeak;
  //       if (ixStrip < leftPeak_inv)
  //       {
  //         for (int ixPosition = 0; ixPosition < 110; ixPosition++)
  //           setPixelColor(ixStrip + 12, ixPosition, CRGB::Black);
  //       }
  //       else
  //       {
  //         for (int ixPosition = 0; ixPosition < 110; ixPosition++)
  //           setPixelColor(ixStrip + 12, ixPosition, CRGB(CHSV(((ixStrip - 12) * 20), 255, 255)));
  //       }
  //     }
  //     // right channel
  //     else
  //     {
  //       if (ixStrip - 5 > rightPeak)
  //       {
  //         for (int ixPosition = 0; ixPosition < 110; ixPosition++)
  //           setPixelColor(ixStrip + 12, ixPosition, CRGB::Black);
  //       }
  //       else
  //       {
  //         for (int ixPosition = 0; ixPosition < 110; ixPosition++)
  //           setPixelColor(ixStrip + 12, ixPosition, CRGB(CHSV(((ixStrip - 12) * 20), 255, 255)));
  //       }
  //     }
  //   }

  //   FastLED.show();
  // }

  // antenna always has blinking red light at the top
  // "for airplanes" but also so we can always find
  // our way home, no matter what pattern is showing!
  void antennaRedBlinky()
  {
    CRGB rgb = CRGB::Red;
    rgb.fadeToBlackBy(beatsin8(45));

    for (int i = redBlinky; i < 900; i++)
      setAntennaPixel(i, rgb);
  }

  // strongman animation
  void carnivalWhoosh()
  {
    antennaSolidColor(CRGB::Black);
    whipSolidColor(CRGB::Black);

    const CRGB rgbWhooshColor = CRGB::White; // so artsy
    const uint32_t msPerStepConverge = 250;
    const uint32_t msConverge = 6 * msPerStepConverge;
    const uint32_t msTimeInFlight = 3334;
    const uint32_t msTimeRest = 500;
    const uint32_t msTotalLength = msConverge + msTimeInFlight + msTimeRest; // how long this whole animation is
    uint32_t msInCycle = millis() % msTotalLength;                           // how far we are into the current cycle
    uint32_t msInCurrentStage = 0;

    if (msInCycle < msConverge) // bands of light converge from outside in on the whips
    {
      msInCurrentStage = msInCycle;

      uint8_t whip = msInCurrentStage / msPerStepConverge;
      whipSolidColor(rgbWhooshColor, whip);
      whipSolidColor(rgbWhooshColor, 11 - whip);
    }
    else if (msInCycle < msConverge + msTimeInFlight)
    {
      msInCurrentStage = msInCycle - msConverge;
      double time = (msInCurrentStage / 1000.0);
      const double gravity = 9.80665;
      double metersHeight = 16.35 * time - gravity * time * time / 2.0;
      uint32_t pixelsHeight = lround(metersHeight * 60.0);
      const uint32_t slugHeightInPixels = 83; // to match the physical height of the whips in the antenna

      for (int i = pixelsHeight; i < pixelsHeight + slugHeightInPixels; i++)
      {
        if (i < 900)
          setAntennaPixel(i, rgbWhooshColor);
      }
    }
  }

  // top level LED show.
  void loop(
      Controller::ButtonState *pbuttonState,
      uint8_t brightnessWhips,
      uint8_t brightnessAntennas,
      uint8_t leftPeak,
      uint8_t rightPeak)
  {
    g_brightnessWhips = brightnessWhips;
    g_brightnessAntennas = brightnessAntennas;

    if (pbuttonState->cColors == 0)
    {

      // No solid colors pressed - just run the current animation
      carnivalWhoosh();
    }
    else
    {
      solidPattern(pbuttonState);
    }

    antennaRedBlinky();
    FastLED.show();
  }
};
