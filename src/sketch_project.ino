/*
 * Program Name:
 *  CS 207 (Lec 001) - Project
 *  Cyborg Headpiece (cs207-2017-project)
 *  
 * Description:
 *  A cyborg headpiece, by my definition, will be a piece of 
 *   wearable technology, affixed to the head of the wearer, 
 *   but made to look as though it is integrated into the 
 *   flesh of the wearer.
 *   
 *   This sketch will take inputs from an accelerometer and
 *    a microphone, and then react to the inputs by driving
 *    a NeoPixel Ring and NeoPixel Matrix respectively.
 * 
 * Materials
 *  Adafruit Circuit Playground - Developer Edition
 * 
 * Setup:
 *  sample setup comments - to be fixed later
 *  Connect a Black wire from Arduino Uno GND to "-" rail
 *  Connect a Red wire from Arduino Uno 5V to "+" rail
 *  
 *  Connect a Black wire from "-" rail to e30
 *  Connect a Red wire from "+" rail to e29
 *  Connect a Yellow wire from Arduino Uno Pin 9 to e28
 *  Connect a servo (ground, power, signal) to a30, a29, a28
 *  
 *  Connect an LED (from - to +) from "-" rail to h15
 *  Connect a 560 ohm resistor from f15 to f19
 *  Connect a Blue wire from Arduino Uno Pin 13 to h19
 *  
 *  Connect a White wire from "-" rail to e7
 *  Connect a 10k ohm resistor from a7 to a3
 *  Connect a button, one side into e3 and f3, the other side into e1 and f1
 *  Connect a Orange wire from "+" rail to j1
 *  Connect a Yellow wire from Arduino Uno Pin 2 to j3
 *  
 * Date:
 *  2017-03-22
 * 
 * Name:
 *  Richard Yeomans
 * 
 */

// Include Adafruit libraries for operation of Circuit Playground
#include <Adafruit_CircuitPlayground.h>
#include <Wire.h>
#include <SPI.h>

// Serial Communication details
const int baud = 9600;

// Which pins are connected to what
const int pinMatrix = 10;
const int pinRing = 2;

// Stored time counts
unsigned long previousMillisMatrix;
unsigned long previousMillisRing;
unsigned long previousMillisCPNeoPixel;

// Motion delay thresholds
int holdMatrix;
int holdRing;
int holdCPNeoPixel;

// The NeoPixel on the Circuit Playground details
int cpNeoPixel;
int cpNeoPixelColor;

// Sound mechanism details
int soundSensorThresh;
int soundSensorMax;
int soundSensorMin;
const int soundSensorHistorySize = 10;
int soundSensorHistory[soundSensorHistorySize];
int soundSensorHistoryItem;
int soundSensorHistoryMax;
int soundSensorHistoryMin;
int soundSensorHistoryAverage;

// Sound mechanism details - Bars
const int soundBarNeoPixelCount = 10;
boolean soundBarNeoPixelActive[soundBarNeoPixelCount];
int soundBarNeoPixelColorR[soundBarNeoPixelCount];
int soundBarNeoPixelColorG[soundBarNeoPixelCount];
int soundBarNeoPixelColorB[soundBarNeoPixelCount];

int soundBarMax;
int soundBarMin;
int soundBarPeak;
int soundBarRaw;
int soundBarDropRate;


// This is the sound level history
int historySoundSensor[5];

// This is the sound level history count
int historyCountSoundSensor = 5;


// If this is true, debug mode is on
boolean stateDebug = true;


void setup() {
  // run once at beginning

  // prepare serial output
  Serial.begin(baud);
  Serial.println("Begin program");

  // initialize Circuit Playground
  CircuitPlayground.begin();

  // setting initial values for variables
  previousMillisMatrix = 0;
  previousMillisRing = 0;
  previousMillisCPNeoPixel = 0;
  holdMatrix = 150;
  holdRing = 150;
  holdCPNeoPixel = 150;

  cpNeoPixel = 0;

  soundSensorThresh = 338;
  soundSensorHistoryItem = 0;
  for (int i = 0; i < soundSensorHistorySize; i++) {
    soundSensorHistory[i] = soundSensorThresh;
  }

  soundBarMax = 30;
  soundBarMin = 0;
  soundBarPeak = 0;
  soundBarRaw = 0;
  soundBarDropRate = 1;
  historyCountSoundSensor = 0;

  // callibration
  soundSensorMax = soundSensorThresh;
  soundSensorMin = soundSensorThresh;
}


void loop() {
  // run repeatedly

  // determine current time count
  unsigned long currentMillis = millis();

  // detect inputs
  boolean stateSwitch = CircuitPlayground.slideSwitch();
  boolean stateButton1 = CircuitPlayground.leftButton();
  boolean stateButton2 = CircuitPlayground.rightButton();
  int stateSoundSensor = CircuitPlayground.soundSensor();

  // update sound sensor history
  soundSensorHistoryItem++;
  if (soundSensorHistoryItem >= soundSensorHistorySize) {
    soundSensorHistoryItem = 0;
  }
  soundSensorHistory[soundSensorHistoryItem] = stateSoundSensor;

  soundSensorHistoryMax = soundSensorThresh;
  soundSensorHistoryMin = soundSensorThresh;
  for (int i = 0; i < soundSensorHistorySize; i++) {
    soundSensorHistoryMax = max(soundSensorHistoryMax, soundSensorHistory[i]);
    soundSensorHistoryMin = min(soundSensorHistoryMin, soundSensorHistory[i]);
  }
  soundSensorHistoryAverage = soundSensorHistoryMax - soundSensorHistoryMin;

  // figure out sound bars
  soundSensorMax = max(soundSensorMax, stateSoundSensor);
  soundSensorMin = min(soundSensorMin, stateSoundSensor);

  if (soundSensorHistoryAverage > 5) {
    soundBarRaw = min(soundSensorHistoryAverage, soundBarMax);
  } else {
    soundBarRaw = 0;
  }

//  soundBarMax = max(soundBarMax, soundBarRaw);
//  soundBarMin = min(soundBarMin, soundBarRaw);

  if (currentMillis - previousMillisCPNeoPixel > holdCPNeoPixel) {
    previousMillisCPNeoPixel = currentMillis;
    soundBarPeak -= soundBarDropRate;
  }
  soundBarPeak = max(soundBarPeak, soundBarRaw);

  int soundBarPeakCount = map(soundBarPeak, soundBarMin, soundBarMax, 0, soundBarNeoPixelCount);
  int soundBarRawCount = map(soundBarRaw, soundBarMin, soundBarMax, 0, soundBarNeoPixelCount);
  for (int i = 0; i < soundBarNeoPixelCount; i++) {
    if (i < soundBarPeakCount) {
      if (i < soundBarRawCount) {
        soundBarNeoPixelColorR[i] = 0;
        soundBarNeoPixelColorG[i] = 255;
        soundBarNeoPixelColorB[i] = 0;
      } else {
        soundBarNeoPixelColorR[i] = 255;
        soundBarNeoPixelColorG[i] = 0;
        soundBarNeoPixelColorB[i] = 0;
      }
    } else {
      soundBarNeoPixelColorR[i] = 0;
      soundBarNeoPixelColorG[i] = 0;
      soundBarNeoPixelColorB[i] = 0;
    }
  }



/*
    historyCountSoundSensor++;
    if (historyCountSoundSensor >= sizeof(historySoundSensor)) {
      historyCountSoundSensor = 0;
    }
    historySoundSensor[historyCountSoundSensor] = stateSoundSensor - threshSoundSensor;

    int valueSoundSensor = 0;
    for (int i = 0; i < sizeof(historySoundSensor); i++) {
      valueSoundSensor += historySoundSensor[i];
    }
    valueSoundSensor /= sizeof(historySoundSensor);

    stateCPNeoPixel = true;
    cpNeoPixelColor = valueSoundSensor * 25;
    if (cpNeoPixelColor > 255) {
      cpNeoPixelColor = 255;
    }
  } else {
    stateCPNeoPixel = false;
  }

  if (currentMillis - previousMillisCPNeoPixel > holdCPNeoPixel) {
    previousMillisCPNeoPixel = currentMillis;
    cpNeoPixel++;
    if (cpNeoPixel > 10) {
      cpNeoPixel = 0;
    }
  }
*/

  // update all outputs
  for (int i = 0; i < soundBarNeoPixelCount; i++) {
    if (soundBarNeoPixelActive[i]) {
      CircuitPlayground.setPixelColor(i, soundBarNeoPixelColorR[i], soundBarNeoPixelColorG[i], soundBarNeoPixelColorB[i]);
    } else {
      CircuitPlayground.setPixelColor(i, soundBarNeoPixelColorR[i], soundBarNeoPixelColorG[i], soundBarNeoPixelColorB[i]);
    }
  }
/*
  if (stateCPNeoPixel) {
    for (int i = 0; i < 10; i++) {
      CircuitPlayground.setPixelColor(i, CircuitPlayground.colorWheel(cpNeoPixelColor));
    }
  } else {
    CircuitPlayground.clearPixels();
  }
*/

  // send message to serial output
  if (stateDebug) {
    // Sound mechanism details
    Serial.println();
    Serial.print("RAW out: ");
    Serial.print(stateSoundSensor);
    Serial.print("  min: ");
    Serial.print(soundSensorMin);
    Serial.print("  max: ");
    Serial.print(soundSensorMax);
    Serial.print("  thresh: ");
    Serial.println(soundSensorThresh);
    Serial.print("CALC out: ");
    Serial.print(soundBarRaw);
    Serial.print("  min: ");
    Serial.print(soundBarMin);
    Serial.print("  max: ");
    Serial.print(soundBarMax);
    Serial.print("  peak: ");
    Serial.println(soundBarPeak);

//    Serial.print("X: "); Serial.print(CircuitPlayground.motionX());
//    Serial.print(" \tY: "); Serial.print(CircuitPlayground.motionY());
//    Serial.print(" \tZ: "); Serial.print(CircuitPlayground.motionZ());
//    Serial.println(" m/s^2");

    delay(30);
  }
}

