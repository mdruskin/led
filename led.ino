#define LOOP_DELAY 5
#define BRIGHTNESS 32
#define NUM_LEDS 480
#define LED_PIN 4
#define BRIGHTNESS_BUTTON_PIN 6
#define MODE_BUTTON_PIN 7
#define SOUND_PIN 0

#include "FastLED.h"

#include "utils.h"
#include "rainbow.h"
#include "pulse.h"
#include "shimmer.h"
#include "twinkle.h"
#include "wanderer.h"
#include "accordian.h"

CRGB leds[NUM_LEDS];
int program = 0;

void setup() {
  delay(1000);
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  pinMode(BRIGHTNESS_BUTTON_PIN, INPUT);
  pinMode(MODE_BUTTON_PIN, INPUT);
  pinMode(SOUND_PIN, INPUT);
}

//////////////////////////////////////
// ******    MAIN LOOP     ******** //
//////////////////////////////////////
long modeMs = 0;
// 60 minutes
long modePeriod = 60 * 60 * 1000;
//long modePeriod = 30 * 1000;

int currentMode = 0;
int totalModes = 6;
int modeToggle = 1;

int currentPattern = 0;

void usePattern(int n) {
  switch (n) {
    case 0:
      shimmer::draw(leds);
      break;
    case 1:
      movingRainbow::draw(leds, 20);
      break;
    case 2:
      twinkle::draw(leds);
      break;
    case 3:
      pulse::draw(leds);
      break;
    case 4:
      movingRainbow::draw(leds, 3000);
      break;
    case 5:
      wanderer::draw(leds);
      break;
    case 6:
      accordian::draw(leds);
      break;
  }
}

void drawFrame() {
  if (isButtonToggle(modeToggle, MODE_BUTTON_PIN)) {
    currentMode = (currentMode + 1) % (totalModes + 1);
    Serial.print("New Mode:");
    Serial.println(currentMode);
  }
  // demo mode - cycle all of them
  if (currentMode == 0) {
    if (periodToggle(modeMs, modePeriod)) {
      currentPattern = (currentPattern + 1) % totalModes;
    }
    usePattern(currentPattern);
  } else { // or specific pattern
    usePattern(currentMode - 1);
  }
  adjustBrightness();
}

void loop() {
  drawFrame();
  FastLED.show();
  delay(LOOP_DELAY);
}
