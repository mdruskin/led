#define LOOP_DELAY 5
#define BRIGHTNESS 64
#define NUM_LEDS 240
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
int currentMode = 0;
int totalModes = 6;
int modeToggle = 1;
void drawFrame() {
  if (isButtonToggle(modeToggle, MODE_BUTTON_PIN)) {
    currentMode = (currentMode + 1) % totalModes;
    Serial.print("New Mode:");
    Serial.println(currentMode);
  }
  switch (currentMode) {
    case 0:
      movingRainbow::draw(leds, 20);
      break;
    case 1:
      movingRainbow::draw(leds, 3000);
      break;
    case 2:
      twinkle::draw(leds);
      break;
    case 3:
      shimmer::draw(leds);
      break;
    case 4:
      pulse::draw(leds);
      break;
    case 5:
      wanderer::draw(leds);
      break;
  }
  adjustBrightness();
}

void loop() {
  drawFrame();
  FastLED.show();
  delay(LOOP_DELAY);
}
