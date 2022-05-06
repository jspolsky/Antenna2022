#include <Arduino.h>

// void setup()
// {
//   // put your setup code here, to run once:
//   pinMode(13, OUTPUT);
// }

// void loop()
// {
//   // put your main code here, to run repeatedly:
//   digitalWrite(13, HIGH);
//   delay(100);
//   digitalWrite(13, LOW);
//   delay(900);
// }

#include <OctoWS2811.h>

const int numPins = 1;
byte pinList[numPins] = {
    33,
};
const int ledsPerStrip = 300;

DMAMEM int displayMemory[ledsPerStrip * 6];
int drawingMemory[ledsPerStrip * 6];

const int config = WS2811_RGB | WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config, numPins, pinList);

void setup()
{
  leds.begin();
  leds.show();
}

// normal:
#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF
#define YELLOW 0xFFFF00
#define PINK 0xFF1088
#define ORANGE 0xE05800
#define WHITE 0xFFFFFF

// half:
// #define RED 0x800000
// #define GREEN 0x008000
// #define BLUE 0x000080
// #define YELLOW 0x808000
// #define PINK 0x800544
// #define ORANGE 0xE05800
// #define WHITE 0x808080

// Less intense...
/*
#define RED    0x160000
#define GREEN  0x001600
#define BLUE   0x000016
#define YELLOW 0x101400
#define PINK   0x120009
#define ORANGE 0x100400
#define WHITE  0x101010
*/

void colorWipe(int color, int wait)
{
  for (int i = 0; i < leds.numPixels(); i++)
  {
    leds.setPixel(i, color);
    leds.show();
    delayMicroseconds(wait);
  }
}

void loop()
{
  int microsec = 2000000 / leds.numPixels(); // change them all in 2 seconds

  // uncomment for voltage controlled speed
  // millisec = analogRead(A9) / 40;

  colorWipe(RED, microsec);
  colorWipe(GREEN, microsec);
  colorWipe(BLUE, microsec);
  colorWipe(WHITE, microsec);

  delay(10000);
}
