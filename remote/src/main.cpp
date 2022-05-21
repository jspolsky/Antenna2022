#include <Arduino.h>
#include <Bounce2.h>

#include "Util.h"
#include "led.h"

// BUTTONS:

const uint8_t pinStart = 25, cpins = 16;
Bounce rgbounce[cpins];

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

  Led::setup();
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

  Led::loop();
}
