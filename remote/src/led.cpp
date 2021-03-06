
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

    FastLED.show();
  }

};
