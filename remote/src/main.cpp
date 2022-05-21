#include <Arduino.h>
#include <Bounce2.h>
#include <OctoWS2811.h>
#include "Util.h"

// BUTTONS:

const uint8_t pinStart = 25, cpins = 16;
Bounce rgbounce[cpins];

// NEOPIXELS:

const int numPins = 1;
byte pinList[numPins] = {
    24,
};
const int ledsPerStrip = 16;

DMAMEM int displayMemory[ledsPerStrip * 6];
int drawingMemory[ledsPerStrip * 6];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config, numPins, pinList);

#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF
#define YELLOW 0xFFFF00
#define PINK 0xFF1088
#define ORANGE 0xE05800
#define WHITE 0xFFFFFF

void colorWipe(int color, int wait)
{
  for (int i = 0; i < leds.numPixels(); i++)
  {
    leds.setPixel(i, color);
    leds.show();
    delayMicroseconds(wait);
  }
}

void setup()
{
  Util::setup();
  for (uint8_t pin = pinStart; pin < pinStart + cpins; pin++)
  {
    rgbounce[pin - pinStart].attach(pin, INPUT_PULLUP);
    rgbounce[pin - pinStart].interval(5);
  }

  dbgprintf("Ready\n");
  Serial1.begin(500000);

  leds.begin();
  leds.show();
}

void loop()
{
  for (uint8_t i = 0; i < cpins; i++)
  {
    rgbounce[i].update();
    if (rgbounce[i].changed())
    {
      bool debouncedInput = (rgbounce[i].read() == LOW);
      byte msg = i | (debouncedInput << 4);
      dbgprintf("%d %d %x\n", i, debouncedInput, msg);
      Serial1.write(msg);
    }
  }

  int microsec = 2000000 / leds.numPixels(); // change them all in 2 seconds

  colorWipe(RED, microsec);
  colorWipe(GREEN, microsec);
  colorWipe(BLUE, microsec);
  colorWipe(WHITE, microsec);
}
