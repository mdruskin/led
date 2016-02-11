#include "FastLED.h"
#include "utils.h"

#ifndef __TWINKLE_H__
#define __TWINKLE_H__

namespace twinkle {
  byte heat[NUM_LEDS];
  byte hue[NUM_LEDS];

  // how many and how quickly new twinkles appear
  byte maxNewTwinkles = 7;
  byte additionalSoundTwinkles = 25;
  int twinklePeriodMillis = 250;
  long lastTwinkleMillis = 0;
  long lastSoundMillis = 0;

  // coling - how fast the twinkles dim
  byte cooling = 1;
  int coolingPeriodMillis = 30;
  long lastCoolingMillis = 0;

  // hsv: 0 - red, 64 - yellow, 96 - green, 160 - blue
  byte startHue = 128; // range of twinkle hues will start from this hue
  byte hueRange = 40; // and go up this many hues
  byte hueIncrement = 1; // and every huePeriodMillis it will increment by this much
  int huePeriodMillis = 4000;
  long lastHueMillis = 0;

  void draw(CRGB leds[NUM_LEDS]) {
    // Cool everything down
    if (periodToggle(lastCoolingMillis, coolingPeriodMillis)) {
      for (int i = 0; i < NUM_LEDS; i++) {
        heat[i] = qsub8(heat[i], cooling); // subtract until 0
      }
    }

    // add new twinkles
    bool soundBurst = isSoundOn() > 254 && periodToggle(lastSoundMillis, twinklePeriodMillis);
    if (periodToggle(lastTwinkleMillis, twinklePeriodMillis) || soundBurst) {
      random16_add_entropy(random()); // improves randomness

      int numTwinkles = random16(0, maxNewTwinkles);
      if (soundBurst) numTwinkles += additionalSoundTwinkles;

      for (int i = 0; i < numTwinkles; i++) {
        byte twinkleHeat = random8(50, 255);
        int twinkleLocation = random16(1, NUM_LEDS - 1);
        byte twinkleHue = random8(startHue, startHue + hueRange);
        heat[twinkleLocation] = twinkleHeat;
        heat[twinkleLocation + 1] = twinkleHeat / 2;
        heat[twinkleLocation - 1] = twinkleHeat / 2;

        hue[twinkleLocation] = twinkleHue;
        hue[twinkleLocation + 1] = twinkleHue;
        hue[twinkleLocation - 1] = twinkleHue;
      }
    }

    // change the hues for the twinkles
    if (periodToggle(lastHueMillis, huePeriodMillis)) {
      startHue += hueIncrement;
    }

    // now set the leds
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(hue[i], 255, heat[i]);
    }
  }
}

#endif
