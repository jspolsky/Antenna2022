#include <Arduino.h>

#include "Util.h"
#include "brightness.h"
#include "led.h"
#include "audioshow.h"
#include "controller.h"

void setup()
{
  Util::setup();
  Brightness::setup();
  Led::setup();
  Audioshow::setup();
  Controller::setup();
}

void loop()
{

  Audioshow::loop();

  uint8_t brightnessWhips, brightnessAntennas;
  Brightness::loop(&brightnessWhips, &brightnessAntennas);

  bool *colorStates = Controller::loop();
  Led::loop(colorStates,
            brightnessWhips,
            brightnessAntennas,
            Audioshow::leftPeak,
            Audioshow::rightPeak);
}
