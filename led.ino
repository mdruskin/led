#include "FastLED.h"

#define NUM_LEDS 240
#define LED_PIN 4
#define BRIGHTNESS_BUTTON_PIN 6
#define MODE_BUTTON_PIN 7
#define SOUND_PIN 8
#define LOOP_DELAY 1
#define BRIGHTNESS 64

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

//////////////////////////////////////
// ******      Pulse       ******** //
//////////////////////////////////////
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
void pulse() {      
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

//////////////////////////////////////
// ******    shimmer       ******** //
//////////////////////////////////////
byte heatShimmer[NUM_LEDS];
bool heatShimmerUpDown[NUM_LEDS];
long lastShimmerMillis = 0;
byte shimmerPeriod = 10;
byte shimmerDiff = 1;
byte shimmerHue = 160;
long lastShimmerHueMillis = 0;
int shimmerHuePeriod = 4000;

void shimmer() {
  if (!periodToggle(lastShimmerMillis, shimmerPeriod))
    return;
    
  if (heatShimmer[0] == 0) {
    // setup
    for (int i = 0; i < NUM_LEDS; i++) {
      heatShimmer[i] = 200;
      heatShimmerUpDown[i] = random8(2);
    }
  }

  // now raise heat up and down
  for (int i = 0; i < NUM_LEDS; i++) {
    // randomly change direction for some of them
    if (random8(100) < 1) // 1% of the time
      heatShimmerUpDown[i] = !heatShimmerUpDown[i];
      
    if (heatShimmerUpDown[i] && heatShimmer[i] < 256 - shimmerDiff)
      heatShimmer[i] += shimmerDiff;
    if (!heatShimmerUpDown[i] && heatShimmer[i] > 64 + shimmerDiff)
      heatShimmer[i] -= shimmerDiff;
  }    

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(shimmerHue, 255, heatShimmer[i]);
  }

  if (periodToggle(lastShimmerHueMillis, shimmerHuePeriod)) {
    shimmerHue++;
  }
}

//////////////////////////////////////
// ******     twinkle      ******** //
//////////////////////////////////////
byte heat[NUM_LEDS];
byte hue[NUM_LEDS];

// how many and how quickly new twinkles appear
byte maxNewTwinkles = 7;
byte additionalSoundTwinkles = 25;
int twinklePeriodMillis = 250;
long lastTwinkleMillis = 0;
long lastSoundMillis = 0;

// coling - how fast the twinkles dim
byte cooling = 1;
int coolingPeriodMillis = 30;
long lastCoolingMillis = 0;

// hsv: 0 - red, 64 - yellow, 96 - green, 160 - blue
byte startHue = 128; // range of twinkle hues will start from this hue
byte hueRange = 40; // and go up this many hues
byte hueIncrement = 1; // and every huePeriodMillis it will increment by this much
int huePeriodMillis = 4000;
long lastHueMillis = 0;

void twinkle() {  
  random16_add_entropy(random()); // improves randomness
  
  // Cool everything down
  if (periodToggle(lastCoolingMillis, coolingPeriodMillis)) { 
    for (int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8(heat[i], cooling); // subtract until 0
    }
  }

  // add new twinkles
  bool soundBurst = isSoundOn(0) && periodToggle(lastSoundMillis, twinklePeriodMillis);
  if (periodToggle(lastTwinkleMillis, twinklePeriodMillis) || soundBurst) { 
    int numTwinkles = random16(0, maxNewTwinkles);
    if (soundBurst) numTwinkles += additionalSoundTwinkles;
      
    for (int i = 0; i < numTwinkles; i++) {
      byte twinkleHeat = random16(50, 255);
      byte twinkleLocation = random16(1, NUM_LEDS - 1);
      byte twinkleHue = random16(startHue, startHue + hueRange); 
      heat[twinkleLocation] = twinkleHeat;
      heat[twinkleLocation + 1] = twinkleHeat / 2;
      heat[twinkleLocation - 1] = twinkleHeat / 2;

      hue[twinkleLocation] = twinkleHue;
      hue[twinkleLocation + 1] = twinkleHue;
      hue[twinkleLocation - 1] = twinkleHue;
    }
  }

  // change the hues for the twinkles
  if (periodToggle(lastHueMillis, huePeriodMillis)) { 
    startHue += hueIncrement;
  }
  
  // now set the leds
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue[i], 255, heat[i]);
  }
}

//////////////////////////////////////
// ********* movingRainbow ******** //
//////////////////////////////////////
// moveEveryMillis is how long between each time
// the rainbow advances one led 10 is fast, 1000 is slow
int startPosition = 0;
long lastMillis = 0;
void movingRainbow(int moveEveryMillis) {
  long millisPassed = periodToggle(lastMillis, moveEveryMillis);
  if (millisPassed > 0) {    
    startPosition += millisPassed / moveEveryMillis;
  }
      
  fill_rainbow(leds, NUM_LEDS, startPosition, 1);
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

//////////////////////////////////////
// ******    MAIN LOOP     ******** //
//////////////////////////////////////
int currentMode = 4;
int modeToggle = 1;
void drawFrame() {
  if (isButtonToggle(modeToggle, MODE_BUTTON_PIN)) {
    currentMode = (currentMode + 1) % 5;
    Serial.print("New Mode:");
    Serial.println(currentMode);
  }
  if (currentMode == 0) {
    movingRainbow(15);
  }
  if (currentMode == 1) {    
    movingRainbow(4000);
  }
  if (currentMode == 2) {
    twinkle();
  }
  if (currentMode == 3) {
    shimmer();
  }
  if (currentMode == 4) {
    pulse();
  }
  adjustBrightness();
}

void loop() {
  drawFrame();
  FastLED.show();
  delay(LOOP_DELAY);
}

