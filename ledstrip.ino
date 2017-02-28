#include <FastLED.h>
#ifdef __AVR__
#include <avr/power.h>
#include <avr/eeprom.h>
#endif

#include "math.h"

#define NUM_LEDS 30
#define BRIGHTNESS 255
#define LEDS_PIN 1

typedef enum {
  MODE_OFF,
  MODE_RAINBOW,
  MODE_FLAME,
  MODE_NOISE1,
  MODE_NOISE2,
  MODE_NOISE3,
  MODE_NOISE4,
  MODE_NOISE5,
  MODE_EOF,
} mode_t;

CRGB leds[NUM_LEDS];
uint8_t lastMillis = 0;
uint8_t slowness = 1;
uint8_t rainbowColor = 0;
uint8_t mode = MODE_RAINBOW;
uint8_t flameHeat[NUM_LEDS];
uint32_t noiseYScale = 0;
#define FLAME_SPEED (1000 / 60) // 60fps, or 16ms per frame
#define FLAME_COOLING 30
#define FLAME_SPARKING 120
CRGBPalette16 palette;


const TProgmemRGBPalette16 RedBlueColors_p FL_PROGMEM =
{
    0xff6600,
    0xff3300,
    0xff0000,
    0xff0011,
    0xff0044,
    0xee0077,
    0xbb00aa,
    0x8800dd,
    0x6600ff,
    0x4400ff,
    0x2200ff,
    0x0000ff,
    0x0000ff,
    0x2222ff,
    0x4444ff,
    0x6666ff
};

void ledstripSetup() {
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  FastLED.addLeds<WS2812, LEDS_PIN, GRB>(leds, NUM_LEDS);
  mode = eeprom_read_byte((unsigned char*)(1));
  slowness = eeprom_read_byte((unsigned char*)(2));
  ledstripUpdateMode();
}

void ledstripLoop() {
  switch (mode) {
    case MODE_OFF:
      memset(leds, 0, NUM_LEDS * 3);
      //for (uint8_t i = 0; i < NUM_LEDS; i++) {
      //  leds[i] = CRGB::Black;
      //}
      break;

    case MODE_RAINBOW: {
      uint8_t currentMillis = millis();
      if (uint8_t(currentMillis - lastMillis) >= uint8_t(7 << slowness)) {
        lastMillis = currentMillis;
        rainbowColor++;
      }

      uint8_t color = rainbowColor;
      uint8_t i = NUM_LEDS;
      do {
        i--;
        leds[i] = CHSV {
          color,
          0xff,
          0xff,
        };
        color += 3;
      } while (i);

      break;
    }

    case MODE_FLAME: {
      // Source:
      // https://github.com/FastLED/FastLED/blob/master/examples/Fire2012/Fire2012.ino
      uint8_t currentMillis = millis();
      if (uint8_t(currentMillis - lastMillis) > FLAME_SPEED) {
        lastMillis = currentMillis;

        // Step 2.  Heat from each cell drifts 'up' and diffuses a little
        for (uint8_t i = NUM_LEDS - 1; i >= 2; i--) {
          flameHeat[i] = (flameHeat[i - 1] + flameHeat[i - 2] + flameHeat[i - 2] ) / 3;
        }

        // Step 3.  Randomly ignite new 'sparks' of flameHeat near the bottom
        if (random8() < FLAME_SPARKING) {
          uint8_t y = random8(4);
          flameHeat[y] = qadd8( flameHeat[y], random8(160, 255) );
        }

        // Step 4.  Map from heat cells to LED colors
        for (uint8_t i = 0; i < NUM_LEDS; i++) {
          //uint8_t colorindex = scale8(flameHeat[i], 200);
          //leds[i] = ColorFromPalette(palette, colorindex);
          leds[i] = HeatColor(flameHeat[i]);

          // Step 1.  Cool down every cell a little
          // Optimization: do this in the same loop here.
          flameHeat[i] = qsub8(flameHeat[i], random8(0, (FLAME_COOLING + 2)));
        }
      }
      break;
    }

    default: // NOISE 1-4
      // Move along the Y axis (time).
      uint8_t currentMillis = millis();
      if (uint8_t(currentMillis - lastMillis) > 8) {
        lastMillis = currentMillis;
        //noiseYScale++;
        noiseYScale += uint16_t(63) << slowness;
      }

      uint8_t i = NUM_LEDS;
      do {
        i--;
        // X location is constant, but we move along the Y at the rate of millis()
        //uint8_t index = inoise8(uint16_t(i) * 20, noiseYScale);
        //leds[i] = ColorFromPalette(palette, index);
        int16_t index = inoise16(uint32_t(i) << 12, noiseYScale);
        leds[i] = ColorFromPalette16(palette, index);
      } while (i);
      break;
  }

  FastLED.show();
}

void ledstripNextSpeed() {
  slowness--;
  if (slowness > 5) { // overflow
    slowness = 5;
  }
  eeprom_write_byte((unsigned char*)(2), slowness);
}

void ledstripNextMode() {
  mode++;
  ledstripUpdateMode();
  eeprom_write_byte((unsigned char*)(1), mode);
}

void ledstripUpdateMode() {
  if (mode >= MODE_EOF) {
    mode = 0; // first mode: MODE_RAINBOW
  }
  switch (mode) {
    case MODE_NOISE1:
      palette = LavaColors_p;
      break;
    case MODE_NOISE2:
      palette = OceanColors_p;
      break;
    case MODE_NOISE3:
      palette = ForestColors_p;
      break;
    case MODE_NOISE4:
      palette = RainbowColors_p;
      break;
    case MODE_NOISE5:
      palette = RedBlueColors_p;
      break;
  }
}
