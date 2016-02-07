#include "FastLED.h"
#include "utils.h"

#ifndef __WANDERER_H__
#define __WANDERER_H__

#define numWanderers 20

namespace wanderer {
  bool isInitialized = false;

  long currentMillis = 0;
  int periodMillis = 100;

  long bgMillis = 0;
  int bgPeriodMillis = 2000;

  byte bgHue = 0;

  int currents[numWanderers];
  bool directions[numWanderers];
  byte lengths[numWanderers];
  int hues[numWanderers];
  int speeds[numWanderers];

  void init(CRGB leds[NUM_LEDS]) {
    for (int i = 0; i < numWanderers; i ++) {
      currents[i] = random16(NUM_LEDS);
      hues[i] = random16();
      speeds[i] = random16(1, 12);
    }
  }

  void bg(CRGB leds[NUM_LEDS]) {
    if (periodToggle(bgMillis, bgPeriodMillis))
      bgHue ++;

    for (int i = 0; i < NUM_LEDS; i ++) {
      leds[i] = CHSV(bgHue, 255, 64);
    }
  }

  void draw(CRGB leds[NUM_LEDS]) {
    if (!isInitialized) {
      init(leds);
      isInitialized = true;
    }

    if (!periodToggle(currentMillis, periodMillis)) return;

    bg(leds);

    // update lengths and speeds
    for (int i = 0; i < numWanderers; i ++) {
      if (lengths[i] == 0) {
        lengths[i] = random8(5, 25);
        if (random8(2) == 0) {
          directions[i] = true;
        } else {
          directions[i] = false;
        }
      }

      // speed
      speeds[i] --;
      if (speeds[i] <= 0) {
        currents[i] = (currents[i] + (directions[i] ? 1 : -1) + NUM_LEDS) % NUM_LEDS;
        lengths[i] --;

        speeds[i] = random8(1, 6);
      }
    }

    // actually draw it
    for (int i = 0; i < numWanderers; i ++) {
      leds[currents[i]] = CHSV(hues[i], 255, 255);
    }
  }
}

#endif
