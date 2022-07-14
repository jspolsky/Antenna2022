#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "util.h"
#include "TEMPLATE.h"

namespace Controller
{
    static bool colorStates[8] = {};

    void setup()
    {
        Serial1.begin(500000);
        //        dbgprintf("receiver ready\n");
    }

    bool *loop()
    {
        int incoming_byte = 0;
        if (Serial1.available() > 0)
        {
            incoming_byte = Serial1.read();
            //            dbgprintf("Serial read: %x\n", incoming_byte);

            if ((incoming_byte & 0xF) < 8)
            {
                colorStates[incoming_byte & 0xF] = (incoming_byte & 0x10) ? 1 : 0;
            }
        }

        return colorStates;
    }
}