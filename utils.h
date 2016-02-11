#include "FastLED.h"

#ifndef __UTILS_H__
#define __UTILS_H__

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
// returns how many elements in the past 255 samples
// are smaller than the current sound. 250 is a good threshold
double average = 1;
int soundHistory[255];
int historyIndex = 0;
int soundTotal = 0;
int isSoundOn() {
  int currentSound = analogRead(SOUND_PIN);

  // adjust average
  //soundTotal = soundTotal - soundHistory[historyIndex];
  //soundTotal = soundTotal + currentSound;
  soundHistory[historyIndex] = currentSound;
  historyIndex = (historyIndex + 1) % 255;

  soundTotal = 0;
  int moreThan = 0;
  for (int i = 0; i < 255; i++) {
    soundTotal += soundHistory[i];
    if (soundHistory[i] < currentSound)
      moreThan++;
  }

  average = (double) soundTotal / 255.0;

  if (moreThan > 250) {
    Serial.println(currentSound);
  }

  return moreThan;
}

//////////////////////////////////////
// ****** adjustBrightness ******** //
//////////////////////////////////////
int brightnessButtonToggle = 1;
byte brightness = 4;
byte brightnessValues[] = { 4, 8, 16, 32, 64, 128, 255 };
void adjustBrightness() {
  if (isButtonToggle(brightnessButtonToggle, BRIGHTNESS_BUTTON_PIN)) {
    brightness = brightness + 1;
    if (brightness > 6) brightness = 0;
    FastLED.setBrightness(brightnessValues[brightness]);
  }
}

// true if the previous color was different from new color, false otherwise
bool setIf(CRGB leds[NUM_LEDS], int i, CHSV newValue) {
  if (leds[i] != newValue) {
    leds[i] = newValue;
    return true;
  }
  return false;
}

#endif
