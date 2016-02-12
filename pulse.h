#include "FastLED.h"
#include "utils.h"

#ifndef __PULSE_H__
#define __PULSE_H__

namespace pulse {

  int pulseLength;
  int currentLed;
  long lastPulseMillis = 0;
  int pulsePeriod = 1000;
  long lastPulseMoveMillis = 0;
  int pulseMovePeriod = 2;
  bool up = true;
  byte basePulseHue = 0;
  byte pulseHue;
  byte backgroundHue = 160;
  long lastBackgroundMillis = 0;
  int backgroundPeriod = 4000;
  byte backgroundHeat = 128;
  int pulseCenters[] = {59, 176, 298, 414};

  void draw(CRGB leds[NUM_LEDS]) {
    if (periodToggle(lastPulseMillis, pulsePeriod)) {
      // generate new pulse length and hue
      pulseLength = random16(30, 40);
      currentLed = 0;
      pulseHue = random8(basePulseHue, basePulseHue + 24);
      up = true;
    }

    // if we reached the top, go down for all 4
    if (currentLed >= pulseLength) {
      up = false;
    }

    // if we are at the bottom, just fill backfround
    if (!up && currentLed <= 0) {
      fill_solid(leds, NUM_LEDS, CHSV(backgroundHue, 255, backgroundHeat));
      return;
    }
    
    if (periodToggle(lastPulseMoveMillis, pulseMovePeriod)) {
      // background
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(backgroundHue, 255, backgroundHeat);
      }

      // tail of the pulses
      for (int i = 0; i < currentLed; i++) {
        byte value = 128 - i * (128 / pulseLength);
        if (up) {            
          for (int p = 0; p < 4; p++) {
            leds[pulseCenters[p] + i] = CHSV(pulseHue, 255, value);
            leds[pulseCenters[p] - i] = CHSV(pulseHue, 255, value);
          }
        } else {
          for (int p = 0; p < 4; p++) {
            leds[pulseCenters[p] + i] = CHSV(pulseHue, 255, value * 0.8);
            leds[pulseCenters[p] - i] = CHSV(pulseHue, 255, value * 0.8);
          }

        }
      }

      // heads of the pulses
      for (int p = 0; p < 4; p++) {
        leds[pulseCenters[p] + currentLed] = CHSV(pulseHue, 220, 255);
        leds[pulseCenters[p] + currentLed + 1] = CHSV(pulseHue, 220, 255);
        leds[pulseCenters[p] - currentLed] = CHSV(pulseHue, 220, 255);
        leds[pulseCenters[p] - currentLed - 1] = CHSV(pulseHue, 220, 255);
      }

      if (up)
        currentLed += 3;
      else
        currentLed -= 2;
    }

    if (periodToggle(lastBackgroundMillis, backgroundPeriod)) {
      basePulseHue++;
      backgroundHue++;
    }
  }
}

#endif
