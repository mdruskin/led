#include "FastLED.h"
#include "utils.h"

#ifndef __WANDERER_H__
#define __WANDERER_H__

namespace accordian {
  const int WIDTH = 20;
  const int NUM_COLORS = 8;
  
  long backgroundMs = 0;
  long foregroundMs = 0;
  long BACKGROUND_MS = 100;
  long FOREGROUND_MS = 100;

  CHSV colors[NUM_COLORS] = {
    CHSV(HUE_RED, 255, 64),
    CHSV(HUE_ORANGE, 255, 64),
    CHSV(HUE_YELLOW, 255, 64),
    CHSV(HUE_GREEN, 255, 64),
    CHSV(HUE_AQUA, 255, 64),
    CHSV(HUE_BLUE, 255, 64),
    CHSV(HUE_PURPLE, 255, 64),
    CHSV(HUE_PINK, 255, 64)
  };
  int positions[NUM_COLORS];
  int thresholds[NUM_COLORS];
  //CHSV pattern[WIDTH];

  int currentPosition = 0;
  int MAX_JUMP = NUM_COLORS;

  bool isInitialized = false;
  
  CHSV backgroundColor = CHSV(HUE_PINK, 0, 0);
  CHSV climberColor = CHSV(HUE_RED, 0, 64);

  void drawBackground(CRGB leds[NUM_LEDS]) {
    for (int i = 0; i < NUM_LEDS; i ++) {
      leds[i] = backgroundColor;
    }
  }

  void drawForeground(CRGB leds[NUM_LEDS]) {
    for (int i = 0; i < NUM_COLORS; i ++) {
      leds[positions[i]] = colors[i];
    }
  }

  bool allAtThreshold(int positions[NUM_COLORS], int threshold) {
    for (int i = 0; i < NUM_COLORS; i ++)
      if (positions[i] == threshold)
        return false;
    return true;
  }

  int qaddn(int a, int b, int maxV) {
    if (a + b > maxV) return maxV;
    return a + b;
  }

  void beat() {
    // set the intervals
    //led[currentPosition] = CHSV(HUE_PINK, 0, 255);
    //led[(currentPosition + WIDTH) % NUM_LEDS] = CHSV(HUE_PINK, 0, 255);

    int end = currentPosition + WIDTH;
    if (allAtThreshold(positions, end)) {
      currentPosition += WIDTH;
    }
    
    for (int i = 0; i < NUM_LEDS; i ++) {
      int wtf = random16(1, MAX_JUMP);
      positions[i] = qaddn(positions[i], wtf, end);
    }
  }

  void draw(CRGB leds[NUM_LEDS]) {
    if (!isInitialized) {
      for (int i = 0; i < NUM_COLORS; i ++) {
        positions[i] = 0;
      }
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

namespace climb {
  const int NUM_CLIMBERS = 10;
  const int DIST_BETWEEN = 1;
  
  long backgroundMs = 0;
  long foregroundMs = 0;
  long BACKGROUND_MS = 100;
  long FOREGROUND_MS = 500;

  int currentHead = 0;
  
  CHSV backgroundColor = CHSV(HUE_PINK, 255, 64);
  CHSV climberColor = CHSV(HUE_RED, 0, 64);

  void drawBackground(CRGB leds[NUM_LEDS]) {
    for (int i = 0; i < NUM_LEDS; i ++) {
      leds[i] = backgroundColor;
    }
  }

  void drawClimbers(CRGB leds[NUM_LEDS]) {
    for (int climber = 0; climber < NUM_CLIMBERS; climber ++) {
      //leds[(currentHead + (climber * (DIST_BETWEEN + 1))) % NUM_LEDS] = climberColor;
      setIf(
        leds,
        (currentHead + (climber * (DIST_BETWEEN + 1))) % NUM_LEDS,
        climberColor
      );
    }
  }

  void updateClimbers() {
    // update state
    currentHead = (currentHead + 1) % NUM_LEDS;
  }

  void draw(CRGB leds[NUM_LEDS]) {
    if (periodToggle(backgroundMs, BACKGROUND_MS)) {
      drawBackground(leds);
      drawClimbers(leds);
    }

    if (periodToggle(foregroundMs, FOREGROUND_MS)) {
      updateClimbers();
    }
  }
}

namespace volume_climb {
  const int NUM_CLIMBERS = 10;
  const int DIST_BETWEEN = 1;
  
  long backgroundMs = 0;
  long foregroundMs = 0;
  long BACKGROUND_MS = 100;
  long FOREGROUND_MS = 500;

  long volumeMs = 0;
  long VOLUME_MS = 100;

  int climbers[NUM_CLIMBERS] = {
    0, 2, 4, 6, 8,
    10, 12, 14, 16, 18
  };
  int hues[NUM_CLIMBERS] = {
    0, 20, 40, 60, 80,
    100, 120, 140, 160, 180
  };
  int thresholds[NUM_CLIMBERS] = {
    0, 20, 40, 60, 80,
    100, 120, 140, 160, 180
  };
  
  CHSV backgroundColor = CHSV(HUE_PINK, 255, 64);
  CHSV climberColor = CHSV(HUE_RED, 0, 64);

  int MAX_VOLUME = 1024;
  int CURRENT_VOLUME = 500;

  void drawBackground(CRGB leds[NUM_LEDS]) {
    for (int i = 0; i < NUM_LEDS; i ++) {
      leds[i] = backgroundColor;
    }
  }

  void drawClimbers(CRGB leds[NUM_LEDS]) {
    for (int climber = 0; climber < NUM_CLIMBERS; climber ++) {
      setIf(leds, climbers[climber], climberColor);
    }
  }

  void updateClimbers() {
    // update state
    //currentHead = (currentHead + 1) % NUM_LEDS;
    for (int climber = 0; climber < NUM_CLIMBERS; climber ++) {
      if (CURRENT_VOLUME > MAX_VOLUME * thresholds[climber]) {
        
      }
    }
  }

  void draw(CRGB leds[NUM_LEDS]) {
    if (periodToggle(backgroundMs, BACKGROUND_MS)) {
      drawBackground(leds);
      drawClimbers(leds);
    }

    if (periodToggle(foregroundMs, FOREGROUND_MS)) {
      updateClimbers();
    }
  }
}

namespace round_and_round {
  const int NUM_COLOURS = 8;

  long currentMillis = 0;
  long periodMillis = 333;

  int currentColour = 0;
  // colour constants from:
  // https://github.com/FastLED/FastLED/wiki/Pixel-reference#chsv
  CHSV colours[NUM_COLOURS] = {
    CHSV(HUE_RED, 255, 64),
    CHSV(HUE_ORANGE, 255, 64),
    CHSV(HUE_YELLOW, 255, 64),
    CHSV(HUE_GREEN, 255, 64),
    CHSV(HUE_AQUA, 255, 64),
    CHSV(HUE_BLUE, 255, 64),
    CHSV(HUE_PURPLE, 255, 64),
    CHSV(HUE_PINK, 255, 64)
  };

  void draw(CRGB leds[NUM_LEDS]) {
    if (!periodToggle(currentMillis, periodMillis)) return;

    for (int i = 0; i < NUM_LEDS; i ++) {
      leds[i] = colours[currentColour];
    }

    currentColour = (currentColour + 1) % NUM_COLOURS;
  }
}

namespace wanderer {
  const int NUM_WANDERERS = 20;

  bool isInitialized = false;

  long currentMillis = 0;  
  int periodMillis = 200;

  long bgMillis = 0;
  int bgPeriodMillis = 2000;

  byte bgHue = 0;

  int currents[NUM_WANDERERS];
  bool directions[NUM_WANDERERS];
  byte lengths[NUM_WANDERERS];
  int hues[NUM_WANDERERS];
  int speeds[NUM_WANDERERS];

  void init(CRGB leds[NUM_LEDS]) {
    for (int i = 0; i < NUM_WANDERERS; i ++) {
      currents[i] = random8(NUM_LEDS);
      hues[i] = random8();
      speeds[i] = random8(1, 12);
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
    for (int i = 0; i < NUM_WANDERERS; i ++) {
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
      int newSpeed = 1;//random8(1, 6);
      if (speeds[i] <= 0) {
        currents[i] = (currents[i] + (directions[i] ? 1 : -1) + NUM_LEDS) % NUM_LEDS;
        lengths[i] --;

        //speeds[i] = random8(1, 6);
        speeds[i] = newSpeed;
      }
    }

    // actually draw it
    for (int i = 0; i < NUM_WANDERERS; i ++) {
      leds[currents[i]] = CHSV(hues[i], 0, 255);
    }
  }
}

#endif
