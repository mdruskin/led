#include "FastLED.h"
#include "utils.h"

#ifndef __PULSE_H__
#define __PULSE_H__

namespace pulse {

  int pulseLength = 0;
  int currentLed = 0;
  long lastPulseMillis = 0;
  int pulsePeriod = 1000;
  long lastPulseMoveMillis = 0;
  int pulseMovePeriod = 2;
  bool up = true;
  byte basePulseHue = 0;
  byte pulseHue;
  byte backgroundPulseHue = 128;
  long lastPulseBackgroundMillis = 0;
  int pulseBackgroundPeriod = 4000;
  byte backgroundPulseHeat = 64;

  void draw(CRGB leds[NUM_LEDS]) {
    if (periodToggle(lastPulseMillis, pulsePeriod)) {
      // generate new pulse length and hue
      pulseLength = random16(60, 160);
      pulseHue = random8(basePulseHue, basePulseHue + 16);
      currentLed = 0;
      up = true;
    }

    // if we reached the top, go down
    if (currentLed >= pulseLength)
      up = false;

    // if we are at the bottom, just fill backfround
    if (!up && currentLed <= 0) {
      fill_solid(leds, NUM_LEDS, CHSV(backgroundPulseHue, 255, backgroundPulseHeat));
      return;
    }

    if (periodToggle(lastPulseMoveMillis, pulseMovePeriod)) {
      // tail of the pulse
      for (int i = 0; i < currentLed; i++) {
        byte value = 128 - i * (128 / pulseLength);
        if (up) {
          leds[i] = CHSV(pulseHue, 255, value);
        } else {
          leds[i] = CHSV(backgroundPulseHue, 255, backgroundPulseHeat);
        }
      }
      // head of the pulse
      leds[currentLed] = CHSV(pulseHue, 220, 255);

      // background
      for (int i = currentLed + 1; i < NUM_LEDS; i++) {
        leds[i] = CHSV(backgroundPulseHue, 255, backgroundPulseHeat);
      }

      if (up)
        currentLed += pulseLength / 25;
      else
        currentLed -= pulseLength / 40;
    }

    if (periodToggle(lastPulseBackgroundMillis, pulseBackgroundPeriod)) {
      basePulseHue++;
      backgroundPulseHue++;
    }
  }
}

#endif
