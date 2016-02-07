#include "FastLED.h"
#include "utils.h"

#ifndef __ACCORDIAN_H__
#define __ACCORDIAN_H__

namespace accordian {
  const int WIDTH = 40;
  const int NUM_COLORS = 8;

  bool isInitialized = false;

  long backgroundMs = 0;
  long foregroundMs = 0;
  long BACKGROUND_MS = 10;
  long FOREGROUND_MS = 10;

  CHSV backgroundColor = CHSV(HUE_BLUE, 255, 0);

  CHSV colors[NUM_COLORS] = {
    CHSV(HUE_RED, 255, 128),
    CHSV(HUE_ORANGE, 255, 128),
    CHSV(HUE_YELLOW, 255, 128),
    CHSV(HUE_GREEN, 255, 128),
    CHSV(HUE_AQUA, 255, 128),
    CHSV(HUE_BLUE, 255, 128),
    CHSV(HUE_PURPLE, 255, 128),
    CHSV(HUE_PINK, 255, 128)
  };
  int offsets[NUM_COLORS];
  int thresholds[NUM_COLORS];

  int currentBrightness = 1;
  int MIN_BRIGHTNESS = 4;
  int thresholdValues[NUM_COLORS + 1] = {
    0,
    64,
    64,
    128,
    128,
    196,
    196,
    255,
    255
  };

  // the higher this is, the more likely to jump
  byte MIN_JUMP = 128;
  byte jumpProbs[NUM_COLORS];

  // **** **** **** **** **** **** **** **** internal utils

  int qaddn(int a, int b, int maxV) {
    if (a + b > maxV) return maxV;
    return a + b;
  }

  int countAtThreshold(int offsets[NUM_COLORS], int threshold) {
    int n = 0;
    for (int i = 0; i < NUM_COLORS; i ++)
      if (offsets[i] >= threshold)
        n ++;
    return n;
  }

  // **** **** **** **** **** **** **** **** drawing

  void drawBackground(CRGB leds[NUM_LEDS]) {
    for (int i = 0; i < NUM_LEDS; i ++) {
      leds[i] = backgroundColor;
    }
  }

  void drawForeground(CRGB leds[NUM_LEDS]) {
    int numAtThreshold = countAtThreshold(offsets, WIDTH);
    CHSV thresholdColor = CHSV(0, 0, thresholdValues[numAtThreshold]);

    // tile the pattern
    for (int tile = 0; tile < NUM_LEDS / WIDTH; tile ++) {
      for (int i = 0; i < NUM_COLORS; i ++) {
        int currentLed = (tile * WIDTH) + offsets[i];
        leds[currentLed] = colors[i];
      }

      // white out the destinations
      leds[tile * WIDTH] = thresholdColor;
    }
  }

  // **** **** **** **** **** **** **** **** pattern logic

  void init() {
    for (int i = 0; i < NUM_COLORS; i ++) {
      offsets[i] = 0;
      jumpProbs[i] = qadd8(random8(), MIN_JUMP);
    }
  }

  void beat() {
    int numAtThreshold = countAtThreshold(offsets, WIDTH);
    if (numAtThreshold == NUM_COLORS) {
      init();
    }

    currentBrightness = numAtThreshold;

    byte randomJump = random8();
    for (int i = 0; i < NUM_COLORS; i ++) {
      if (randomJump < jumpProbs[i]) {
        offsets[i] = qaddn(offsets[i], 1, WIDTH);
      }
    }
  }

  void draw(CRGB leds[NUM_LEDS]) {
    if (!isInitialized) {
      init();

      isInitialized = true;
    }

    if (periodToggle(backgroundMs, BACKGROUND_MS)) {
      drawBackground(leds);
      drawForeground(leds);
    }

    if (periodToggle(foregroundMs, FOREGROUND_MS)) {
      beat();
    }
  }
}

#endif
