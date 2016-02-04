#include "FastLED.h"

#ifndef __UTILS_H__
#define __UTILS_H__

#define NUM_LEDS 240
#define LED_PIN 4
#define BRIGHTNESS_BUTTON_PIN 6
#define MODE_BUTTON_PIN 7
#define LOOP_DELAY 1
#define BRIGHTNESS 64
#define SOUND_PIN 8

//////////////////////////////////////
// ********* isButtonToggle ******* //
//////////////////////////////////////
// buttonToggle: a global flag that should start off at 1
// pin: the pin id of the button
bool isButtonToggle(int &buttonToggle, int pin) {
  int buttonValue = digitalRead(pin);
  if (buttonValue != buttonToggle && buttonValue == 0) {
    buttonToggle = buttonValue;
    return true;
  }
  buttonToggle = buttonValue;
  return false;
}

//////////////////////////////////////
// *********  periodToggle  ******* //
//////////////////////////////////////
// lastMillis: a global long to keep track of millis
// periodMillis: how often should the period trigger
// returns: time since lastMillis, or 0 if period didn't trigger
int periodToggle(long &lastMillis, int periodMillis) {
  long currentMillis = millis();
  long millisPassed = currentMillis - lastMillis;
  if (millisPassed >= periodMillis) {
    lastMillis = currentMillis;
    return millisPassed;
  }
  return 0;
}

//////////////////////////////////////
// *********   isSoundOn    ******* //
//////////////////////////////////////
// returns true if sound is on and will
// continue returning true for a specified period
long lastMillisSound = 0;
bool isSoundOn(int soundEffectLength) {
  bool result = false;
  long currentMillis = millis();
  if ((currentMillis - lastMillisSound < soundEffectLength)) {
    result = true;
  }
  if (digitalRead(SOUND_PIN)) {
    result = true;
    lastMillisSound = currentMillis;
  }
  return result;
}

#endif
