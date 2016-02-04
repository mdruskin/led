#include "FastLED.h"
#include "utils.h"

#ifndef __WANDERER_H__
#define __WANDERER_H__

#define numWanderers 25

namespace wanderer {
  long saturationMillis = 0;
  long w_millis = 0;
  byte w_length = 0;
  bool w_direction = true;
  int w_current = 150;
  byte w_val = 100;
  bool w_inited = false;
  int w_currents[numWanderers];
  bool w_directions[numWanderers];
  byte w_lengths[numWanderers];
  int w_hues[numWanderers];
  int w_speeds[numWanderers];
  byte w_bg_hue = 0;
  int w_bg_rate = 4;

  void go(CRGB leds[NUM_LEDS]) {
    if (!w_inited) {
      for (int i = 0; i < numWanderers; i ++) {
        w_currents[i] = random8();
        w_hues[i] = random8();
        w_speeds[i] = random8(1, 12);
      }
      w_inited = true;
    }

    //if (!periodToggle(w_millis, 333)) return;
    if (!periodToggle(w_millis, 88)) return;

    w_bg_rate --;
    if (w_bg_rate <= 0) {
      w_bg_hue ++;
      for (int i = 0; i < NUM_LEDS; i ++) {
        leds[i] = CHSV(w_bg_hue, 255, 64);
      }
      w_bg_rate = 4;
    }

    //int hue = 255;

    for (int i = 0; i < numWanderers; i ++) {
      if (w_lengths[i] == 0) {
        w_lengths[i] = random8(5, 25);
        //w_direction = random8(2) == 0 ? 1 : -1;
        if (random8(2) == 0) {
          w_directions[i] = true;
        } else {
          w_directions[i] = false;
        }
      }

      // speed
      w_speeds[i] --;
      if (w_speeds[i] <= 0) {
        w_currents[i] = (w_currents[i] + (w_directions[i] ? 1 : -1) + NUM_LEDS) % NUM_LEDS;
        w_lengths[i] --;

        w_speeds[i] = random8(1, 6);
      }
    }

    /*
    if (w_length == 0) {
      w_length = random8(5, 25);
      //w_direction = random8(2) == 0 ? 1 : -1;
      if (random8(2) == 0) {
        w_direction = true;
      } else {
        w_direction = false;
      }
    }
    w_current = (w_current + (w_direction ? 1 : -1) + NUM_LEDS) % NUM_LEDS;
    w_length --;
    */

    for (int i = 0; i < numWanderers; i ++) {
      leds[w_currents[i]] = CHSV(w_hues[i], 255, 255);
    }
  }
}

#endif
