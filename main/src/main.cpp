#include <Arduino.h>

#include "Util.h"
#include "brightness.h"
#include "controller.h"
#include "led.h"
#include "audioshow.h"

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

  Controller::ButtonState *pbuttonState = Controller::loop();
  Led::loop(pbuttonState,
            brightnessWhips,
            brightnessAntennas,
            Audioshow::leftPeak,
            Audioshow::rightPeak);
}
