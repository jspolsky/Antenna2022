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
    AudioAnalyzeFFT1024 fft1024;

    AudioConnection c1(audioInput, 0, peak_L, 0);
    AudioConnection c2(audioInput, 1, fft1024, 0);
    AudioConnection c3(audioInput, 1, peak_R, 0);

    AudioControlSGTL5000 audioShield;

    float leftPeak = 0.0;
    float rightPeak = 0.0;
    float fftBands[6];

    void setup()
    {
        AudioMemory(12);
        audioShield.enable();
        audioShield.inputSelect(myInput);
        audioShield.volume(0.5);

        fft1024.windowFunction(AudioWindowHanning1024);

        pinMode(5, INPUT_PULLUP);

        for (int i = 0; i < 6; i++)
            fftBands[i] = 0.0;
    }

    bool jack_plugged_in()
    {
        return digitalRead(5);
    }

    void loop()
    {
        if (peak_L.available() && peak_R.available())
        {
            leftPeak = peak_L.read();
            rightPeak = peak_R.read();
        }

        if (fft1024.available())
        {
            fftBands[0] = fft1024.read(0, 1);
            fftBands[1] = fft1024.read(2, 5);
            fftBands[2] = fft1024.read(6, 13);
            fftBands[3] = fft1024.read(14, 29);
            fftBands[4] = fft1024.read(30, 61);
            fftBands[5] = fft1024.read(62, 128);
        }
    }

};