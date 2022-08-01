#include <Arduino.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "Util.h"
#include "audioshow.h"

namespace Audioshow
{

    const int myInput = AUDIO_INPUT_LINEIN;

    AudioInputI2S audioInput; // audio shield: mic or line-in
    AudioAnalyzePeak peak_L;
    AudioAnalyzePeak peak_R;
    AudioOutputI2S audioOutput; // audio shield: headphones & line-out

    AudioConnection c1(audioInput, 0, peak_L, 0);
    AudioConnection c2(audioInput, 1, peak_R, 0);
    AudioConnection c3(audioInput, 0, audioOutput, 0);
    AudioConnection c4(audioInput, 1, audioOutput, 1);

    AudioControlSGTL5000 audioShield;

    void setup()
    {
        AudioMemory(6);
        audioShield.enable();
        audioShield.inputSelect(myInput);
        audioShield.volume(0.5);

        pinMode(5, INPUT_PULLUP);
    }

    bool jack_plugged_in()
    {
        return digitalRead(5);
    }

    float leftPeak = 0.0;
    float rightPeak = 0.0;

    void loop()
    {
        if (peak_L.available() && peak_R.available())
        {
            leftPeak = peak_L.read();
            rightPeak = peak_R.read();
        }
    }

};