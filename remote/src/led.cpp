
#include <Arduino.h>
#include <OctoWS2811.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "Teensy4Controller.h"
#include "Util.h"
#include "led.h"

namespace Led
{

  //
  // Physical arrangement of LEDs
  //

  const int numPins = 1;
  byte pinList[numPins] = {
      24,
  };

  byte hues[8] = {8, 136, 79, 192, 0, 242, 48, 178};
  byte saturations[8] = {255, 255, 255, 255, 255, 192, 255, 255};

  const int ledsPerStrip = 16;
  CRGB pixels[numPins * ledsPerStrip];

  // These buffers need to be large enough for all the pixels.
  // The total number of pixels is "ledsPerStrip * numPins".
  // Each pixel needs 3 bytes, so multiply by 3.  An "int" is
  // 4 bytes, so divide by 4.  The array is created using "int"
  // so the compiler will align it to 32 bit memory.
  DMAMEM int displayMemory[ledsPerStrip * numPins * 3 / 4];
  int drawingMemory[ledsPerStrip * numPins * 3 / 4];
  OctoWS2811 octo(ledsPerStrip, displayMemory, drawingMemory, WS2811_RGB | WS2811_800kHz, numPins, pinList);
  CTeensy4Controller<GRB, WS2811_800kHz> *pcontroller;

  void setup()
  {
    octo.begin();
    pcontroller = new CTeensy4Controller<GRB, WS2811_800kHz>(&octo);

    FastLED.setBrightness(255);

    // FastLED.setCorrection(TypicalLEDStrip);
    // FastLED.setTemperature(DirectSunlight);

    FastLED.addLeds(pcontroller, pixels, numPins * ledsPerStrip);
  }

  void loop()
  {
    for (int i = 0; i < 8; i++)
    {
      pixels[i] = CHSV(hues[i], saturations[i], 255);
    }

    // UFO
    static uint8_t hueUFO = 0;
    EVERY_N_MILLIS(5)
    {
      hueUFO = (hueUFO + 1) % 256;
    }
    pixels[8] = CHSV(hueUFO, 255, 192);

    // monochrome
    uint32_t msInCycle = millis() % 3000;
    if (msInCycle < 2000)
    {
      pixels[9] = CRGB::Blue;
    }
    else
    {
      pixels[9] = CRGB::Black;
    }

    // carnival
    msInCycle = millis() % 1000;
    CRGB color = CRGB::White;
    if (msInCycle < 500)
    {
      color.fadeLightBy(map(msInCycle, 0, 500, 0, 255));
    }
    pixels[10] = color;

    // pride
    const uint32_t num_colors = 11;
    static CRGB prideColors[num_colors] =
        {
            CRGB::Red,
            CHSV(16, 255, 255),
            CHSV(64, 255, 255),
            CHSV(108, 255, 128),
            CRGB::DarkBlue,
            CHSV(198, 255, 128),
            CRGB::White,
            CHSV(224, 255, 255),
            CHSV(144, 255, 255),
            CHSV(16, 255, 64),
            CRGB::Black,
        };
    static int currentColor = 0;
    EVERY_N_MILLIS(300)
    {
      currentColor = (currentColor + 1) % num_colors;
    }
    pixels[11] = prideColors[currentColor];

    // renegade
    static uint8_t hueRenegade = 0;
    EVERY_N_MILLIS(1)
    {
      hueRenegade = (hueRenegade + 1) % 256;
    }
    pixels[12] = CHSV(hueRenegade, 255, 255);

    // seagreen
    static uint8_t hueSea = 0;
    EVERY_N_MILLIS(10)
    {
      hueSea = (hueSea + 1) % 256;
    }
    pixels[13] = CHSV(map(hueSea, 0, 255, 96, 180), 255, 255);

    // trans trance
    const uint32_t trans_num_colors = 3;
    static CRGB transColors[trans_num_colors] =
        {
            CRGB::White,
            CHSV(224, 255, 255),
            CHSV(144, 255, 255),
        };
    static int trans_currentColor = 0;
    EVERY_N_MILLIS(300)
    {
      trans_currentColor = (trans_currentColor + 1) % trans_num_colors;
    }
    pixels[14] = transColors[trans_currentColor];

    pixels[15] = HeatColor(beatsin8(80));

    FastLED.show();
  }

};
