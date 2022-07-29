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

  static bool audioMode = false;

  EVERY_N_MILLIS(100)
  {
    audioMode = Audioshow::jack_plugged_in();
  }

  Audioshow::loop();

  uint8_t brightnessWhips, brightnessAntennas;
  Brightness::loop(&brightnessWhips, &brightnessAntennas);

  Controller::ButtonState *pbuttonState = Controller::loop();
  Led::loop(pbuttonState,
            brightnessWhips,
            brightnessAntennas,
            audioMode,
            Audioshow::leftPeak,
            Audioshow::rightPeak);
}
