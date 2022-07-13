#include <Arduino.h>

#include "Util.h"
#include "brightness.h"
#include "led.h"
#include "audioshow.h"

void setup()
{
  Util::setup();
  Brightness::setup();
  Led::setup();
  Audioshow::setup();

  Serial1.begin(500000);
  dbgprintf("receiver ready\n");
}

void loop()
{

  Audioshow::loop();

  uint8_t brightnessWhips, brightnessAntennas;
  Brightness::loop(&brightnessWhips, &brightnessAntennas);
  Led::loop(brightnessWhips, brightnessAntennas,
            Audioshow::leftPeak, Audioshow::rightPeak);

  int incoming_byte = 0;
  if (Serial1.available() > 0)
  {
    incoming_byte = Serial1.read();
    dbgprintf("Serial read: %x\n", incoming_byte);
  }
}
