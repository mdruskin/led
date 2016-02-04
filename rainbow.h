#include "FastLED.h"
#include "utils.h"

#ifndef __RAINBOW_H__
#define __RAINBOW_H__

namespace movingRainbow {
  int startPosition = 0;
  long lastMillis = 0;

  // moveEveryMillis is how long between each time
  // the rainbow advances one led.
  // 10 is fast, 2000 is slow
  void draw(CRGB leds[NUM_LEDS], int moveEveryMillis) {
    long millisPassed = periodToggle(lastMillis, moveEveryMillis);
    if (millisPassed > 0) {
      startPosition += millisPassed / moveEveryMillis;
    }

    fill_rainbow(leds, NUM_LEDS, startPosition, 1);
  }
}

#endif
