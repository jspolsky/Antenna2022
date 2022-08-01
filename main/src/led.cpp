
#include <Arduino.h>
#include <OctoWS2811.h>
#include "Teensy4Controller.h"
#include "Util.h"
#include "controller.h"
#include "led.h"

namespace Led
{

  //
  // Physical arrangement of LEDs
  //

  const int numPins = 24;
  byte pinList[numPins] = {
      2, 9, 24,   // ANT - North - Bottom to Top
      27, 28, 31, // ANT - East - Bottom to Top
      3, 4, 25,   // ANT - South - Bottom to Top
      26, 29, 30, // ANT - West - Bottom to Top

      32, 35, 36, 39, 40, 22, // WHIPS - Left - Right
      33, 34, 37, 38, 41, 14, // WHIPS continued
  };

  const int ledsPerStrip = 300;
  CRGB pixels[numPins * ledsPerStrip];

  // These buffers need to be large enough for all the pixels.
  // The total number of pixels is "ledsPerStrip * numPins".
  // Each pixel needs 3 bytes, so multiply by 3.  An "int" is
  // 4 bytes, so divide by 4.  The array is created using "int"
  // so the compiler will align it to 32 bit memory.
  DMAMEM int displayMemory[ledsPerStrip * numPins * 3 / 4];
  int drawingMemory[ledsPerStrip * numPins * 3 / 4];
  OctoWS2811 octo(ledsPerStrip, displayMemory, drawingMemory, WS2811_RGB | WS2811_800kHz, numPins, pinList);
  CTeensy4Controller<RGB, WS2811_800kHz> *pcontroller;

  uint8_t g_brightnessWhips = 0, g_brightnessAntennas = 0;
  const int redBlinky = 891; // where the red blinking light for the top starts

  void setup()
  {
    octo.begin();
    pcontroller = new CTeensy4Controller<RGB, WS2811_800kHz>(&octo);
    FastLED.setBrightness(255); // we do our own dimming

    // FastLED.setCorrection(TypicalLEDStrip);
    // FastLED.setTemperature(DirectSunlight);
    FastLED.setDither(0);
    FastLED.addLeds(pcontroller, pixels, numPins * ledsPerStrip);
  }

  //
  // USEFUL PIXEL-MAPPING UTILITY FUNCTIONS
  //

  void setPixelColor(int ixStrip, int ixPosition, CRGB rgb, uint8_t brightness)
  {
    pixels[ixStrip * ledsPerStrip + ixPosition] = rgb % brightness;
  }

  // set a pixel on the antenna; 0 is the bottom, 899 is the top.
  // Specify a direction 0 - 3 or leave out for all sides
  void setAntennaPixel(uint16_t pixel, CRGB rgb, int oneDirection = 4)
  {
    for (int direction = 0; direction < 4; direction++)
    {
      if (oneDirection == 4 || oneDirection == direction)
        setPixelColor(pixel / 300 + 3 * direction, pixel % 300, rgb, pixel >= redBlinky ? 255 : g_brightnessAntennas);
    }
  }

  // set a pixel on the whips; 0 is the bottom, 110 is the top.
  // Specify a whip 0 - 11 or leave out for all whips
  void setWhipPixel(uint16_t pixel, CRGB rgb, int oneWhip = 12)
  {
    for (int whip = 0; whip < 12; whip++)
    {
      if (oneWhip == 12 || oneWhip == whip)
        setPixelColor(whip + 12, pixel, rgb, g_brightnessWhips);
    }
  }

  void antennaSolidColor(CRGB rgb)
  {
    for (uint16_t i = 0; i < 900; i++)
    {
      setAntennaPixel(i, rgb);
    }
  }

  void whipSolidColor(CRGB rgb, int oneWhip = 12)
  {
    for (uint16_t i = 0; i < 110; i++)
    {
      setWhipPixel(i, rgb, oneWhip);
    }
  }

  void fadeWhipToBlack(int oneWhip, uint8_t by)
  {
    fadeToBlackBy(pixels + ((oneWhip + 12) * ledsPerStrip), 110, by);
  }

  // Some solid color buttons are being pressed; show those colors
  //
  void solidPattern(Controller::ButtonState *pbuttonState)
  {
    // the ANTENNA always goes solid with the first color
    antennaSolidColor(pbuttonState->rgrgb[0]);

    // only 1 color? The whips will join it!
    if (pbuttonState->cColors == 1)
    {
      whipSolidColor(pbuttonState->rgrgb[0]);
    }
    else
    {
      int whichColor = 1;

      // otherwise the whips get divided up among the remaining colors
      for (int oneWhip = 0; oneWhip < 12; oneWhip++)
      {
        whipSolidColor(pbuttonState->rgrgb[whichColor], oneWhip);
        whichColor++;
        if (whichColor == pbuttonState->cColors)
          whichColor = 1;
      }
    }
  }

  // not very excited by this VU meter. But it's something
  void whipAudio(float leftPeak, float rightPeak)
  {

    EVERY_N_MILLIS(50)
    {
      // just do left channel for the moment

      static float maxPeakSeen = 0.0;

      // scale it to 0-12 to represent position on LEDs
      float leftPeakScaled = leftPeak * 14.0 - 2.0;
      if (leftPeakScaled > maxPeakSeen)
      {
        maxPeakSeen = leftPeakScaled;
      }
      else
      {
        maxPeakSeen = maxPeakSeen - 0.25;
      }

      whipSolidColor(CRGB(0, 0, 16));

      for (int ixWhip = 0; ixWhip < leftPeakScaled; ixWhip++)
      {
        whipSolidColor(CRGB(255, 0, 0), ixWhip);
      }

      int maxPeakSeenI = min(lround(maxPeakSeen), 11);
      whipSolidColor(CRGB(0, 128, 0), (maxPeakSeenI));
    }
  }

  // this is much more fun
  void whipAudioMotion(float leftPeak, float rightPeak)
  {
    static float rgLeft[12] =
        {
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        };
    static float rgRight[12] =
        {
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        };

    EVERY_N_MILLIS(50)
    {
      for (int i = 0; i < 11; i++)
      {
        rgLeft[i] = rgLeft[i + 1];
        rgRight[i] = rgRight[i + 1];
      }

      rgLeft[11] = leftPeak;
      rgRight[11] = rightPeak;

      whipSolidColor(CRGB::Black);

      for (int whip = 0; whip < 12; whip++)
      {
        // whip height is 110
        long leftHeightInPixels = lround(rgLeft[whip] * 55.0);
        for (int j = 55; j < 55 + leftHeightInPixels && j < 110; j++)
        {
          setWhipPixel(j, CRGB::Red, whip);
        }

        long rightHeightInPixels = lround(rgRight[whip] * 55.0);
        for (int j = 54; j > 54 - rightHeightInPixels && j > 0; j--)
        {
          setWhipPixel(j, CRGB::Green, whip);
        }

        setWhipPixel(55, CRGB::Blue, whip);
      }
    }
  }

  // antenna always has blinking red light at the top
  // "for airplanes" but also so we can always find
  // our way home, no matter what pattern is showing!
  void antennaRedBlinky()
  {
    CRGB rgb = CRGB::Red;
    rgb.fadeToBlackBy(beatsin8(45));

    for (int i = redBlinky; i < 900; i++)
      setAntennaPixel(i, rgb);
  }

  // calculates the pixel location of an object that started flying
  // up from pixel 0 at startSpeed speed (in m/s), after ms time elapsed
  auto pixelLocationAtTime(double startSpeed, long msElapsed)
  {
    double time = (msElapsed / 1000.0);
    const double gravity = 9.80665;
    double metersHeight = startSpeed * time - gravity * time * time / 2.0;
    return lround(metersHeight * 60.0);
  }

  void carnivalWhoosh()
  {
    antennaSolidColor(CRGB::Black);

    EVERY_N_MILLIS(1)
    {
      for (int i = 0; i < 12; i++)
        fadeWhipToBlack(i, 64);
    }

    const CRGB rgbWhooshColor = CRGB::White; // so artsy
    const uint32_t msTimeInFlight = 3334;
    const uint32_t msConverge = 300;
    const uint32_t msTimeRest = 200;
    const uint32_t msTotalLength = msConverge + msTimeInFlight + msTimeRest; // how long this whole animation is
    uint32_t msInCycle = millis() % msTotalLength;                           // how far we are into the current cycle
    uint32_t msInCurrentStage = 0;

    if (msInCycle < msConverge) // bands of light converge from right to left on the whips
    {
      msInCurrentStage = msInCycle;

      // calculate the "position" of this color which is moving in over the course of 6 seconds
      const double dStart = 11.3333; // start position
      const double dEnd = -0.3333;   // end position
      double dPosition = map((double)msInCycle, 0.0, (double)msConverge, dStart, dEnd);
      long lPosition = lround(dPosition);
      if (lPosition >= 0 && lPosition < 12)
      {
        whipSolidColor(CRGB::White, lPosition);
      }
    }
    else if (msInCycle < msConverge + msTimeInFlight)
    {
      msInCurrentStage = msInCycle - msConverge;

      uint32_t pixelsHeight = pixelLocationAtTime(16.35, msInCurrentStage);
      const uint32_t slugHeightInPixels = 83; // to match the physical height of the whips in the antenna

      for (uint32_t i = pixelsHeight; i < pixelsHeight + slugHeightInPixels; i++)
      {
        if (i < 900)
          setAntennaPixel(i, rgbWhooshColor);
      }
    }
  }

  void monochromeBasic()
  {
    static bool initialSetup = false; // true when we set up the first time

    // there are 56 units
    //    0 - 11 are whips
    //    12 - 55 are segments on the antenna (each direction is separate)
    const uint32_t cUnits = 56;
    const uint32_t cUnitsOnAtATime = 17;
    const uint32_t millisRampTime = 200;     // how many ms it takes to warm up
    const uint32_t millisRampDownTime = 400; // how many ms it takes to fade down to black

    static uint32_t millisCycle = millis();

    // each unit is:
    //    off -- just keep it off
    //    on -- just keep it on
    //    going off -- ramp down for 500 ms then off
    //    going on -- ramp up for 50ms then on
    enum Mode
    {
      ModeOff,
      ModeOn,
      ModeGoingOff,
      ModeGoingOn
    };

    static Mode rgmode[cUnits];

    auto findRandomUnitMatching = [&](Mode modeMatch)
    {
      int32_t iRandom = random(0, cUnits);
      while (rgmode[iRandom] != modeMatch)
      {
        iRandom = (iRandom + 1) % cUnits;
      }
      return iRandom;
    };

    if (!initialSetup)
    {

      for (uint32_t ix = 0; ix < cUnits; ix++)
        rgmode[ix] = ModeOff;

      // to start:
      //    pick 17 at random and turn them on. Everything else is off
      for (uint32_t c = 0; c < cUnitsOnAtATime; c++)
      {
        rgmode[findRandomUnitMatching(ModeOff)] = ModeGoingOn;
      };

      initialSetup = true;
    }

    EVERY_N_SECONDS(3)
    {
      millisCycle = millis();

      // every 3 seconds:
      //    find all "going on" and change them to on
      //    find all "going off" and change them to off
      for (uint32_t i = 0; i < cUnits; i++)
      {
        if (rgmode[i] == ModeGoingOff)
          rgmode[i] = ModeOff;
        else if (rgmode[i] == ModeGoingOn)
          rgmode[i] = ModeOn;
      }

      //    pick 7 at random that are OFF and change them to going on
      //    pick 7 at random that are ON and change them to going off
      for (uint32_t c = 0; c < 7; c++)
      {
        rgmode[findRandomUnitMatching(ModeOff)] = ModeGoingOn;
        rgmode[findRandomUnitMatching(ModeOn)] = ModeGoingOff;
      }
    }

    // Ok, enough beating around the bush, let's turn on some LEDs
    antennaSolidColor(CRGB::Black);
    whipSolidColor(CRGB::Black);

    auto setUnitColor = [](CRGB color, uint32_t unit)
    {
      if (unit < 12)
      {
        whipSolidColor(color, unit);
      }
      else
      {
        unit -= 12;
        uint32_t direction = unit % 4;
        unit /= 4;
        uint32_t firstPixel = unit * 81;
        for (uint32_t i = firstPixel; i < (firstPixel + 81) && i < 900; i++)
        {
          setAntennaPixel(i, color, direction);
        }
      }
    };

    for (uint32_t i = 0; i < cUnits; i++)
    {
      if (rgmode[i] == ModeOn)
      {
        setUnitColor(CRGB::Blue, i);
      }
      else if (rgmode[i] == ModeGoingOn)
      {
        uint32_t millisSinceCycle = millis() - millisCycle;
        if (millisSinceCycle < millisRampTime)
        {
          CRGB color = CRGB::Blue;
          color.fadeToBlackBy(map(millisSinceCycle, 0, millisRampTime, 255, 0));
          setUnitColor(color, i);
        }
        else
          setUnitColor(CRGB::Blue, i);
      }
      else if (rgmode[i] == ModeGoingOff)
      {
        uint32_t millisSinceCycle = millis() - millisCycle;
        if (millisSinceCycle < millisRampDownTime)
        {
          CRGB color = CRGB::Blue;
          color.fadeToBlackBy(map(millisSinceCycle, 0, millisRampDownTime, 0, 255));
          setUnitColor(color, i);
        }
      }
    }
  }

  void lgbtq(bool btrans = false)
  {
    const uint32_t num_colors = 11;
    static CRGB prideColors[num_colors] =
        {
            CRGB::Red,
            CHSV(16, 255, 255),
            CHSV(64, 255, 255),
            CHSV(108, 255, 128),
            CRGB::DarkBlue,
            CHSV(198, 255, 128),
            CRGB::White,
            CHSV(224, 255, 255),
            CHSV(144, 255, 255),
            CHSV(16, 255, 64),
            CRGB::Black,
        };
    const uint32_t millisPerCycle = 6000L;
    const uint32_t millisToMoveIn = 350L;

    // what's on the antenna? Starting at the bottom

    static uint8_t rgAntennaState[num_colors] = {
        10,
        9,
        8,
        7,
        6,
        5,
        4,
        3,
        2,
        1,
        0,
    };

    static uint8_t nextColor = 10;
    static uint8_t transNextColor = 0;
    static uint32_t millisCycle = millis();
    static bool bPushInIsOver = true;

    // Every cycle, start moving the next color in
    EVERY_N_MILLISECONDS(millisPerCycle)
    {
      nextColor = (nextColor + 1) % num_colors;
      if (btrans)
      {
        transNextColor = (transNextColor + 1) % 3;
        nextColor = transNextColor + 6;
      }
      millisCycle = millis();
      bPushInIsOver = false;
    }

    EVERY_N_MILLIS(1)
    {
      for (int i = 0; i < 12; i++)
        fadeWhipToBlack(i, 64);
    }

    uint32_t millisInCycle = millis() - millisCycle;
    if (millisInCycle < millisToMoveIn)
    {
      // calculate the "position" of this color which is moving in over the course of 1 second
      const double dStart = 11.3333; // start position
      const double dEnd = -0.3333;   // end position
      double dPosition = map((double)millisInCycle, 0.0, (double)millisToMoveIn, dStart, dEnd);
      long lPosition = lround(dPosition);
      if (lPosition >= 0 && lPosition < 12)
      {
        whipSolidColor(prideColors[nextColor], lPosition);
      }
    }
    else
    {
      if (!bPushInIsOver)
      {
        // just go in here once at the end of millisToMoveIn
        bPushInIsOver = true;

        // rearrange the antenna!
        for (uint32_t i = num_colors - 1; i > 0; i--)
        {
          rgAntennaState[i] = rgAntennaState[i - 1];
        }
        rgAntennaState[0] = nextColor;
      }
    }

    antennaSolidColor(CRGB::Black);

    // show antenna
    for (uint32_t i = 0; i < num_colors; i++)
    {
      long pixelOffset = 0;

      // Is there an offset for this block of the antenna
      // caused by the bounce?
      if (millisInCycle > millisToMoveIn)
        pixelOffset = pixelLocationAtTime((double)i * 2.0, (millisInCycle - millisToMoveIn));

      if (pixelOffset < 0)
        pixelOffset = 0;

      for (uint32_t ixPixel = 81 * i + pixelOffset;
           ixPixel < 81 * (i + 1) + pixelOffset && ixPixel < 900;
           ixPixel++)
      {
        setAntennaPixel(ixPixel, prideColors[rgAntennaState[i]]);
      }
    }
  }

  void renegadeBurn()
  {
    static uint16_t x = 0;
    int scale = 2; // higher numbers: bigger blobs of color. Lower numbers: smaller blobs.
    static uint16_t t = 0;

    for (uint16_t i = 0; i < 900; i++)
    {
      uint8_t noise = inoise8(i / scale + x, t);
      uint8_t hue = map(noise, 50, 190, 0, 255); // spread results out into 0-255 hue range.
                                                 // try other ranges, like 0-64 for orange/yellow or 96-180 for bluegreen https://github.com/FastLED/FastLED/wiki/FastLED-HSV-Colors

      // OPTION ONE - just use hue to get a full-rainbow
      setAntennaPixel(i, CHSV(hue, 255, 255));
      // adjust the second param of beatsin. 164 makes a pronounced throb. 192 is gently throb. 64 is rock and roll
      // OPTION TWO - pick color from a palette
      // pixels[i] = ColorFromPalette(caribbean, hue);

      if (i < 12)
        whipSolidColor(CHSV(hue, 255, 255), i);
    }

    // actually seriously consider adding beatsin8() to the global brightness as a completely independent genome, for
    // all patterns.

    EVERY_N_MILLISECONDS(10)
    {
      // adjusting x slides the whole pattern up and down
      // subtracting from x slides the pattern up the antenna
      // adding to x slides the pattern down the antenna
      x -= 3; // lower numbers: slower. Higher numbers: faster. 10 is kinda average.

      // adjusting t morphs the whole pattern smoothly
      t += 1; // 1 is probably too slow. 10 is about as fast as you can see!
    }
  }

  void sea()
  {
    static uint16_t x = 0;
    int scale = 2; // higher numbers: bigger blobs of color. Lower numbers: smaller blobs.
    static uint16_t t = 0;

    for (uint16_t i = 0; i < 900; i++)
    {
      uint8_t noise = inoise8(i / scale + x, t);
      uint8_t hue = map(noise, 50, 190, 96, 180); // spread results out into 0-255 hue range.
                                                  // try other ranges, like 0-64 for orange/yellow or 96-180 for bluegreen https://github.com/FastLED/FastLED/wiki/FastLED-HSV-Colors

      // OPTION ONE - just use hue to get a full-rainbow
      setAntennaPixel(i, CHSV(hue, 255, 255));
      // adjust the second param of beatsin. 164 makes a pronounced throb. 192 is gently throb. 64 is rock and roll
      // OPTION TWO - pick color from a palette
      // pixels[i] = ColorFromPalette(caribbean, hue);

      if (i < 12)
        whipSolidColor(CHSV(hue, 255, 255), i);
    }

    EVERY_N_MILLISECONDS(10)
    {
      // adjusting x slides the whole pattern up and down
      // subtracting from x slides the pattern up the antenna
      // adding to x slides the pattern down the antenna
      x -= 3; // lower numbers: slower. Higher numbers: faster. 10 is kinda average.

      // adjusting t morphs the whole pattern smoothly
      t += 1; // 1 is probably too slow. 10 is about as fast as you can see!
    }
  }

  void ufo()
  {
    static uint8_t hue_cylon = 0;

    EVERY_N_MILLIS(10)
    {

      uint16_t cypos = scale8(cubicwave8(millis() / 10), 11);
      for (int i = 0; i < 12; i++)
      {
        fadeWhipToBlack(i, 16);
      }
      whipSolidColor(CHSV(hue_cylon, 255, 255), cypos);
    }

    EVERY_N_MILLIS(300)
    {
      hue_cylon++;
    }

    // now the antenna:

    uint16_t x;
    int scale;
    uint16_t t;

    x = 0;
    t = millis() / 5;
    scale = beatsin8(5, 10, 10);

    for (int i = 0; i < 90; i++)
    {
      uint8_t noise = inoise8(i * scale + x, t);
      uint8_t hue = map(noise, 50, 190, 0, 255);
      setAntennaPixel(i * 10, CRGB::Black);
      for (int j = i * 10 + 1; j < (i + 1) * 10; j++)
      {
        setAntennaPixel(j, CHSV(hue, 255, 128));
      }
    }
  }

  void fire()
  {
    // FIRE ON THE WHIPS!

    const int NUM_LEDS = 110;
    const int NUM_WHIPS = 12;
    const int COOLING = 75;
    const int SPARKING = 120;

    static uint8_t heat[NUM_WHIPS][NUM_LEDS];

    for (int whip = 0; whip < NUM_WHIPS; whip++)
    {

      // Step 1.  Cool down every cell a little
      for (int i = 0; i < NUM_LEDS; i++)
      {
        heat[whip][i] = qsub8(heat[whip][i], random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
      }

      // Step 2.  Heat from each cell drifts 'up' and diffuses a little
      for (int k = NUM_LEDS - 1; k >= 2; k--)
      {
        heat[whip][k] = (heat[whip][k - 1] + heat[whip][k - 2] + heat[whip][k - 2]) / 3;
      }

      // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
      if (random8() < SPARKING)
      {
        int y = random8(7);
        heat[whip][y] = qadd8(heat[whip][y], random8(160, 255));
      }

      // Step 4.  Map from heat cells to LED colors
      for (int j = 0; j < NUM_LEDS; j++)
      {
        CRGB color = HeatColor(heat[whip][j]);
        setWhipPixel(j, color, whip);
      }
    }

    // STARS ON THE ANTENNA!

    uint16_t pixel = max(3, random16(900));
    setAntennaPixel(pixel, CRGB(12, 12, 64));
    setAntennaPixel(pixel - 1, CRGB::White);
    setAntennaPixel(pixel - 2, CRGB(12, 12, 64));
    fadeToBlackBy(pixels, 3600, 1);
  }

  // top level LED show.
  void loop(
      Controller::ButtonState *pbuttonState,
      uint8_t brightnessWhips,
      uint8_t brightnessAntennas,
      bool bAudioMode,
      float leftPeak,
      float rightPeak)
  {
    g_brightnessWhips = brightnessWhips;
    g_brightnessAntennas = brightnessAntennas;

    if (pbuttonState->cColors > 0)
    {
      // solid color buttons always work
      solidPattern(pbuttonState);
    }
    else if (bAudioMode)
    {
      // if an audio jack is plugged in,
      // run the audio program
      whipAudioMotion(leftPeak, rightPeak);
    }
    else
    {
      // run the animation
      switch (pbuttonState->animation)
      {
      case 7:
        fire();
        break;

      case 6:
        lgbtq(true);
        break;

      case 5:
        sea();
        break;

      case 4:
        renegadeBurn();
        break;

      case 3:
        lgbtq();
        break;

      case 2:
        carnivalWhoosh();
        break;

      case 1:
        monochromeBasic();
        break;

      case 0:
      default:
        ufo();
        break;
      }
    }

    antennaRedBlinky();
    FastLED.show();
  }
};
