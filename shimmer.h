#include "FastLED.h"
#include "utils.h"

#ifndef __SHIMMER_H__
#define __SHIMMER_H__

namespace shimmer {
  byte heat[NUM_LEDS];
  bool heatUpDown[NUM_LEDS];
  long lastShimmerMillis = 0;
  byte shimmerPeriod = 20;
  byte shimmerDiff = 1;
  byte hue = 160;
  long lastShimmerHueMillis = 0;
  int shimmerHuePeriod = 4000;

  void draw(CRGB leds[NUM_LEDS]) {
    if (!periodToggle(lastShimmerMillis, shimmerPeriod))
      return;

    if (heat[0] == 0) {
      // setup
      for (int i = 0; i < NUM_LEDS; i++) {
        heat[i] = 200;
        heatUpDown[i] = random8(2);
      }
    }

    // now raise heat up and down
    for (int i = 0; i < NUM_LEDS; i++) {
      // randomly change direction for some of them
      if (random8(100) < 1) // 1% of the time
        heatUpDown[i] = !heatUpDown[i];

      if (heatUpDown[i] && heat[i] < 256 - shimmerDiff)
        heat[i] += shimmerDiff;
      if (!heatUpDown[i] && heat[i] > 64 + shimmerDiff)
        heat[i] -= shimmerDiff;
    }

    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(hue, 255, heat[i]);
    }

    if (periodToggle(lastShimmerHueMillis, shimmerHuePeriod)) {
      hue++;
    }
  }
}

#endif
