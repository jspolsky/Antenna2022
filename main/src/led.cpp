
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
      33,
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
  CTeensy4Controller<GRB, WS2811_800kHz> *pcontroller;

  void setup()
  {
    octo.begin();
    pcontroller = new CTeensy4Controller<GRB, WS2811_800kHz>(&octo);

    FastLED.setBrightness(128);

    // FastLED.setCorrection(TypicalLEDStrip);
    // FastLED.setTemperature(DirectSunlight);

    FastLED.addLeds(pcontroller, pixels, numPins * ledsPerStrip);
  }

  void loop()
  {
    int hue = (millis() / 100) % 255;
    int extent = (millis() / 10) % ledsPerStrip;
    for (int i = 0; i < extent; i++)
    {
      pixels[i] = CRGB(CHSV(hue, 255, 255));
    }

    FastLED.show();
  }

};
