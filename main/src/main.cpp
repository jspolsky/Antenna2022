#include <Arduino.h>

#include "Util.h"
#include "led.h"
#include "audioshow.h"

void setup()
{
  Util::setup();

  Led::setup();
  Audioshow::setup();

  Serial1.begin(500000);
  dbgprintf("receiver ready\n");

  pinMode(A2, INPUT); // pot - whips
  pinMode(A3, INPUT); // pot - ant
}

void loop()
{

  Audioshow::loop();
  Led::loop(Audioshow::leftPeak, Audioshow::rightPeak);

  int incoming_byte = 0;
  if (Serial1.available() > 0)
  {
    incoming_byte = Serial1.read();
    dbgprintf("Serial read: %x\n", incoming_byte);
  }

  EVERY_N_MILLIS(1500)
  {
    //    dbgprintf("Pot: %d %d\n", analogRead(A2), analogRead(A3));
  }
}
