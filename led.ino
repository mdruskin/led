#include "FastLED.h"

#define NUM_LEDS 240
#define LED_PIN 4
#define BRIGHTNESS_BUTTON_PIN 6
#define MODE_BUTTON_PIN 7
#define SOUND_PIN 8
#define LOOP_DELAY 10
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
// lasMillis: a global long to keep track of millis
// periodMillis: how often should the period trigger
// returns: time since lastMillis, or 0 if period didn't trigger
int periodToggle(long &lastMillis, int periodMillis) {
  long currentMillis = millis();
  long millisPassed = currentMillis - lastMillis;
  if (millisPassed > periodMillis) {
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
// ******    twinkle       ******** //
//////////////////////////////////////
byte heat[NUM_LEDS];
byte hue[NUM_LEDS];

// how many and how quickly new twinkles appear
byte maxNewTwinkles = 10;
byte additionalSoundTwinkles = 25;
int twinklePeriodMillis = 500;
long lastTwinkleMillis = 0;
long lastSoundMillis = 0;

// coling - how fast the twinkles dim
byte cooling = 2;
int coolingPeriodMillis = 20;
long lastCoolingMillis = 0;

// hsv: 0 - red, 64 - yellow, 96 - green, 160 - blue
byte startHue = 128; // range of twinkle hues will start from this hue
byte hueRange = 40; // and go up this many hues
byte hueIncrement = 5; // and every 10 sec it will increment by this much
int huePeriodMillis = 10000;
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
// *********  movingRedDot  ******* //
//////////////////////////////////////
int dotLocation = 0;
int dotSpeed = 1;
void movingRedDot() {  
  fill_solid(leds, NUM_LEDS, CRGB::Blue);
  int dotColor = CRGB::Red;
  leds[dotLocation] = dotColor;
  leds[dotLocation + 1] = dotColor;
  leds[dotLocation + 2] = dotColor;
  dotLocation += dotSpeed;
  if (dotLocation >= NUM_LEDS - 2)
    dotLocation = 0;
}

//////////////////////////////////////
// ********* movingRainbow ******** //
//////////////////////////////////////
// moveEveryMillis is how long between each time
// the rainbow advances one led 10 is fast, 1000 is slow
int startPosition = 0;
long lastMillis = 0;
void movingRainbow(int moveEveryMillis) {
  long currentMillis = millis();
  long millisPassed = currentMillis - lastMillis;
  if (millisPassed >= moveEveryMillis) {
    lastMillis = currentMillis;
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
int currentMode = 0;
int modeToggle = 1;
void drawFrame() {
  if (isButtonToggle(modeToggle, MODE_BUTTON_PIN)) {
    currentMode = (currentMode + 1) % 3;
    Serial.print("New Mode:");
    Serial.println(currentMode);
  }
  if (currentMode == 0) {
    movingRainbow(20);
  }
  if (currentMode == 1) {    
    movingRainbow(1000);
  }
  if (currentMode == 2) {
    twinkle();
  }
  adjustBrightness();
}

void loop() {
  drawFrame();
  FastLED.show();
  delay(LOOP_DELAY);
}

