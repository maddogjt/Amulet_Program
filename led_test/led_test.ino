// #include <Adafruit_NeoPixel.h>
// #ifdef __AVR__
// #include <avr/power.h>
// #endif

// #include "FastLED.h"

// #define PIN 25 //PIN_RGB_LEDS 

// // Parameter 1 = number of pixels in strip
// // Parameter 2 = Arduino pin number (most are valid)
// // Parameter 3 = pixel type flags, add together as needed:
// //   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
// //   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
// //   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
// //   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
// //   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
// Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(4, PIN, NEO_GRB + NEO_KHZ800);
// Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(4, 16, NEO_GRB + NEO_KHZ800);

// uint16_t numPixels() {
//   return strip1.numPixels() + strip2.numPixels();
// }

// void setPixelColor(uint16_t n, uint32_t c) {
//   if(n < strip1.numPixels()) {
//     // if (n==2 || n == 3) {
//     //   c = 0;
//     // }
//     strip1.setPixelColor(n, c);
//   } else {
//     strip2.setPixelColor(n-strip1.numPixels(), c);
//   }
// }

// void show() {
//   strip1.show();
//   strip2.show();
// }

//     // IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
//     // pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
//     // and minimize distance between Arduino and first pixel.  Avoid connecting
//     // on a live circuit...if you must, connect GND first.

// void setup()
// {
//   Serial.begin(115200);
//   while (!Serial)
//     delay(10); // for nrf52840 with native usb
// // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
// #if defined(__AVR_ATtiny85__)
//   if (F_CPU == 16000000)
//     clock_prescale_set(clock_div_1);
// #endif
//   // End of trinket special code

//   pinMode(PIN, OUTPUT);
//   pinMode(PIN_LED1, OUTPUT);
//   digitalWrite(PIN_LED1, 1);

//   strip1.begin();
//   strip1.setBrightness(5);
//   strip1.show(); // Initialize all pixels to 'off'
//   strip2.begin();
//   strip2.setBrightness(5);
//   strip2.show(); // Initialize all pixels to 'off'
// }

// void loop()
// {
//   // pinMode(31, OUTPUT);
//   // while (1)
//   // {
//   //   static int v = 0;
//   //   v = !v;
//   //   digitalWrite(31, v);
//   //   delay(10);
//   // }

//   // Some example procedures showing how to display to the pixels:
//   colorWipe(strip1.Color(255, 0, 0), 50); // Red
//   Serial.println("next");
//   colorWipe(strip1.Color(0, 255, 0), 50); // Green
//   Serial.println("next");
//   colorWipe(strip1.Color(0, 0, 255), 50); // Blue
//   Serial.println("next");
//                   //colorWipe(strip1.color(0, 0, 0, 255), 50); // White RGBW
//   // Send a theater pixel chase in...
//   theaterChase(strip1.Color(127, 127, 127), 50); // White
//   Serial.println("next");
//   theaterChase(strip1.Color(127, 0, 0), 50);     // Red
//   Serial.println("next");
//   theaterChase(strip1.Color(0, 0, 127), 50); // Blue
//   Serial.println("next");
//   rainbow(20);
//   rainbowCycle(20);
//   theaterChaseRainbow(50);
// }

// // Fill the dots one after the other with a color
// void colorWipe(uint32_t c, uint8_t wait)
// {
//   for (uint16_t i = 0; i < numPixels(); i++)
//   {
//     setPixelColor(i, c);
//     show();
//     delay(wait);
//   }
// }

// void rainbow(uint8_t wait)
// {
//   uint16_t i, j;

//   for (j = 0; j < 256; j++)
//   {
//     for (i = 0; i < numPixels(); i++)
//     {
//       setPixelColor(i, Wheel((i + j) & 255));
//     }
//     show();
//     delay(wait);
//   }
// }

// // Slightly different, this makes the rainbow equally distributed throughout
// void rainbowCycle(uint8_t wait)
// {
//   uint16_t i, j;

//   for (j = 0; j < 256 * 5; j++)
//   { // 5 cycles of all colors on wheel
//     for (i = 0; i < numPixels(); i++)
//     {
//       setPixelColor(i, Wheel(((i * 256 / numPixels()) + j) & 255));
//     }
//     show();
//     delay(wait);
//   }
// }

// //Theatre-style crawling lights.
// void theaterChase(uint32_t c, uint8_t wait)
// {
//   for (int j = 0; j < 10; j++)
//   { //do 10 cycles of chasing
//     for (int q = 0; q < 3; q++)
//     {
//       for (uint16_t i = 0; i < numPixels(); i = i + 3)
//       {
//         setPixelColor(i + q, c); //turn every third pixel on
//       }
//       show();

//       delay(wait);

//       for (uint16_t i = 0; i < numPixels(); i = i + 3)
//       {
//         setPixelColor(i + q, 0); //turn every third pixel off
//       }
//     }
//   }
// }

// //Theatre-style crawling lights with rainbow effect
// void theaterChaseRainbow(uint8_t wait)
// {
//   for (int j = 0; j < 256; j++)
//   { // cycle all 256 colors in the wheel
//     for (int q = 0; q < 3; q++)
//     {
//       for (uint16_t i = 0; i < numPixels(); i = i + 3)
//       {
//         setPixelColor(i + q, Wheel((i + j) % 255)); //turn every third pixel on
//       }
//       show();

//       delay(wait);

//       for (uint16_t i = 0; i < numPixels(); i = i + 3)
//       {
//         setPixelColor(i + q, 0); //turn every third pixel off
//       }
//     }
// }
// }

// // Input a value 0 to 255 to get a color value.
// // The colours are a transition r - g - b - back to r.
// uint32_t Wheel(byte WheelPos)
// {
//   WheelPos = 255 - WheelPos;
//   if (WheelPos < 85)
//   {
//     return strip1.Color(255 - WheelPos * 3, 0, WheelPos * 3);
//   }
//   if (WheelPos < 170)
//   {
//     WheelPos -= 85;
//     return strip1.Color(0, WheelPos * 3, 255 - WheelPos * 3);
//   }
//   WheelPos -= 170;
//   return strip1.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
// }

#include <FastLED.h>
int buttonPin = PIN_DFU;
#include "jsbutton.h"

FASTLED_USING_NAMESPACE

constexpr int singleClick = 1;
constexpr int doubleClick = 2;
constexpr int longHold = 3;

// FastLED "100-lines-of-code" demo reel, showing just a few
// of the kinds of animation patterns you can quickly and easily
// compose using FastLED.
//
// This example also shows one easy way to define multiple
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN PIN_RGB_LEDS
//#define CLK_PIN   4
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS RGB_LED_COUNT
CRGB leds[NUM_LEDS];

#define BRIGHTNESS 8
#define FRAMES_PER_SECOND 120

  void setup()
  {
    pinMode(28, INPUT_PULLUP);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.begin(115200);
    // while (!Serial)
    //    delay(10); // for nrf52840 with native usb

  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined(__AVR_ATtiny85__)
    if (F_CPU == 16000000)
      clock_prescale_set(clock_div_1);
  #endif
    // End of trinket special codex
     //digitalWrite(LED_BUILTIN, HIGH);

  // Serial.println("Wheee!");

  pinMode(PIN_DFU, INPUT_PULLUP);
  pinMode(PIN_RGB_LED_EN, OUTPUT);
  digitalWrite(PIN_RGB_LED_EN, RGB_LED_EN_ON);



  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, RGB_LED_COUNT).setCorrection(TypicalLEDStrip);
  // FastLED.addLeds<LED_TYPE, 16, COLOR_ORDER>(leds + 4, 4).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {
    rainbow,
    red,
    green,
    blue, confetti, sinelon, cycleHSV};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0;                  // rotating "base color" used by many of the patterns

uint32_t lastMillis = 0;
bool ledState = 0;
void loop()
{
 // Serial.println("Looping!");
  if ((millis() - lastMillis) > 2000) {
    lastMillis = millis();
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
    Serial.println("Looping!");
  }


  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  // do some periodic updates
  EVERY_N_MILLISECONDS(20) { gHue++; }   // slowly cycle the "base color" through the rainbow

  uint8_t b = checkButton();

  if (b == singleClick)
  { // Just a click event to advance to next pattern
    // gCurrentPatternNumber = (gCurrentPatternNumber + 1) % maxMode;
    nextPattern();
    Serial.print("Switching pattern to ");
    Serial.println(gCurrentPatternNumber);

  }

  if (digitalRead(PIN_DFU)==0) {
    Serial.println("Going to sleep now");
//    delay(500);

    digitalWrite(PIN_RGB_LED_EN, !RGB_LED_EN_ON);
    digitalWrite(LED_BUILTIN, HIGH);

    systemOff(29, 0);

  }

//  EVERY_N_SECONDS(10) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
}

void red()
{
  fill_solid(leds, NUM_LEDS, CRGB(255, 0, 0));
}

void green()
{
  fill_solid(leds, NUM_LEDS, CRGB(0, 255, 0));
}

void blue()
{
  fill_solid(leds, NUM_LEDS, CRGB(0, 0, 255));
}

void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow(leds, NUM_LEDS, gHue, 20);
}


void addGlitter(fract8 chanceOfGlitter)
{
  if (random8() < chanceOfGlitter)
  {
    leds[random16(NUM_LEDS)] += CRGB::White;
  }
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}
void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  for (int i = 0; i < NUM_LEDS; i++)
  { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle()
{
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, 20);
  byte dothue = 0;
  for (int i = 0; i < 8; i++)
  {
    leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void cycleHSV()
{
  fill_solid(leds, NUM_LEDS, CHSV(gHue, 255, 192));
}