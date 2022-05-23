#include <Arduino.h>

#include "Util.h"
#include "led.h"

void setup()
{
  Util::setup();

  Led::setup();

  Serial1.begin(500000);
  dbgprintf("receiver ready\n");
}

void loop()
{

  Led::loop();

  int incoming_byte = 0;
  if (Serial1.available() > 0)
  {
    incoming_byte = Serial1.read();
    dbgprintf("Serial read: %x\n", incoming_byte);
  }
}
