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
  byte pulseHue[4];
  byte backgroundHue = 160;
  long lastBackgroundMillis = 0;
  int backgroundPeriod = 4000;
  byte backgroundHeat = 128;

  void draw(CRGB leds[NUM_LEDS]) {
    if (periodToggle(lastPulseMillis, pulsePeriod)) {
      // generate new pulse length and hue
      pulseLength = random16(50, 80);
      currentLed = 0;
      for (int i = 0; i < 4; i++) {
        pulseHue[i] = random8(basePulseHue, basePulseHue + 16);        
      }
      up = true;
    }

    // if we reached the top, go down for all 4
    if (currentLed >= pulseLength)
      up = false;

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
          leds[i] = CHSV(pulseHue[0], 255, value);
          leds[240 - i] = CHSV(pulseHue[1], 255, value);
          leds[240 + i] = CHSV(pulseHue[2], 255, value);
          leds[480 - i] = CHSV(pulseHue[3], 255, value);
        } else {
          leds[i] = CHSV(pulseHue[0], 255, value / 4);
          leds[240 - i] = CHSV(pulseHue[1], 255, value / 4);
          leds[240 + i] = CHSV(pulseHue[2], 255, value / 4);
          leds[480 - i] = CHSV(pulseHue[3], 255, value / 4);
        }
      }

      // heads of the pulses
      leds[currentLed] = CHSV(pulseHue[0], 220, 255);
      leds[currentLed + 1] = CHSV(pulseHue[0], 220, 255);
      leds[240 - currentLed] = CHSV(pulseHue[1], 220, 255);
      leds[240 - currentLed - 1] = CHSV(pulseHue[1], 220, 255);
      leds[240 + currentLed] = CHSV(pulseHue[2], 220, 255);
      leds[240 + currentLed + 1] = CHSV(pulseHue[2], 220, 255);
      leds[480 - currentLed] = CHSV(pulseHue[3], 220, 255);
      leds[480 - currentLed - 1] = CHSV(pulseHue[3], 220, 255);

      if (up)
        currentLed += 5;
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
