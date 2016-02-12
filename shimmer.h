#include "FastLED.h"
#include "utils.h"

#ifndef __SHIMMER_H__
#define __SHIMMER_H__

namespace shimmer {
  byte heat[NUM_LEDS];
  bool heatUpDown[NUM_LEDS];
  byte hueMod[NUM_LEDS];
  long lastShimmerMillis = 0;
  byte shimmerPeriod = 50;
  byte hue = 160;
  byte hueModMax = 10;
  long lastShimmerHueMillis = 0;
  int shimmerHuePeriod = 4000;
  bool initialized = false;  

  void init() {
    for (int i = 0; i < NUM_LEDS; i++) {
      heat[i] = 160;
      heatUpDown[i] = random8(2);
      hueMod[i] = random8(hueModMax);
    }
    initialized = true;
  }
  
  void draw(CRGB leds[NUM_LEDS]) {
    if (!periodToggle(lastShimmerMillis, shimmerPeriod))
      return;

    if (!initialized) 
      init();
      
    // now raise heat up and down
    for (int i = 0; i < NUM_LEDS; i++) {
      if (random8(100) < 1) { // 1% of the time 
        // randomly change direction for some of them
        heatUpDown[i] = !heatUpDown[i];
        // randomly change the hueMod
        hueMod[i] = random8(hueModMax);
      }

      // increase brightness more at higher brightness
      // because smaller diffs are less visible at high brightness
      byte shimmerDiff = 1;
      if (heat[i] > 64) shimmerDiff = 2;
      if (heat[i] > 128) shimmerDiff = 4;
      
      if (heatUpDown[i] && heat[i] < 256 - shimmerDiff)
        heat[i] += shimmerDiff;
      if (!heatUpDown[i] && heat[i] > 32 + shimmerDiff)
        heat[i] -= shimmerDiff;
    }

    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(hue + hueMod[i], 255, heat[i]);
    }

    if (periodToggle(lastShimmerHueMillis, shimmerHuePeriod)) {
      hue++;
    }
  }
}

#endif
