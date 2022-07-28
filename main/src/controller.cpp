#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "util.h"
#include "controller.h"

// Interfaces to the "keyboard" with 16 buttons allowing the
// user to interact with the light show.

// Top 8 buttons are solid colors. User can press one or more to
// "take over" the show with their chosen colors.

// Bottom 8 buttons are animated and change the entire light show
// sequence that runs by default.

namespace Controller
{
    // These are the 8 solid colors you can punch up using the buttons on the controller:
    byte hues[8] = {8, 136, 79, 192, 0, 242, 48, 178};
    byte saturations[8] = {255, 255, 255, 255, 255, 192, 255, 255};

    static ButtonState buttonState = {
        0,
        {0, 0, 0, 0, 0, 0, 0, 0},
        {CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black,
         CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black},
        0};

    void setup()
    {
        Serial1.begin(500000);
        // dbgprintf("receiver ready\n");
    }

    ButtonState *loop()
    {
        int incoming_byte = 0;
        if (Serial1.available() > 0)
        {
            incoming_byte = Serial1.read();
            // dbgprintf("Serial read: %x\n", incoming_byte);

            uint8_t whichButton = incoming_byte & 0xF;
            bool bButtonDown = (incoming_byte & 0x10) ? 1 : 0;

            if (whichButton < 8)
            {
                // This is a little complicated because we have to maintain
                // an ordered list of all the buttons that are still pressed, in
                // the chronological order they were pressed

                if (bButtonDown)
                {
                    if (buttonState.cColors < 8)
                    {
                        buttonState.rgbuttons[buttonState.cColors++] = whichButton;
                    }
                }
                else
                {
                    for (int i = 0; i < buttonState.cColors; i++)
                    {
                        if (buttonState.rgbuttons[i] == whichButton)
                        {
                            for (int j = i; j < 7; j++)
                            {
                                buttonState.rgbuttons[j] = buttonState.rgbuttons[j + 1];
                            }
                            buttonState.rgbuttons[7] = 0;
                            buttonState.cColors--;
                        }
                    }
                }

                for (int i = 0; i < 8; i++)
                {
                    buttonState.rgrgb[i] = CHSV(hues[buttonState.rgbuttons[i]],
                                                saturations[buttonState.rgbuttons[i]],
                                                255);
                }
            }
            else
            {
                if (bButtonDown)
                {
                    buttonState.animation = whichButton - 8;
                }
            }
        }

        return &buttonState;
    }
}