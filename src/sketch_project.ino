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
 *  2017-03-25
 * 
 * Name:
 *  Richard Yeomans
 * 
 */

// Include Adafruit libraries for operation of Circuit Playground
#include <Adafruit_CircuitPlayground.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>

// Serial Communication details
const int baud = 9600;

// Which pins are connected to what
const int pinShield = 6;
const int pinMatrix = 10;
const int pinRing = 2;

// Stored time counts
unsigned long previousMillisShield;
unsigned long previousMillisMatrix;
unsigned long previousMillisRing;
unsigned long previousMillisCPNeoPixel;

// Motion delay thresholds
int holdShield;
int holdMatrix;
int holdRing;
int holdCPNeoPixel;

// The NeoPixel on the Circuit Playground details
int cpNeoPixel;
int cpNeoPixelColor;

// Adafruit NeoPixel Shield for Arduino - 40 RGB LED Pixel Matrix details
const int shieldRows = 8;
const int shieldCols = 5;
int shieldTotal = shieldRows * shieldCols;
Adafruit_NeoPixel shield = Adafruit_NeoPixel(shieldTotal, pinShield, NEO_GRB + NEO_KHZ800);
int shieldY;  // corresponds with rows
int shieldX;  // corresponds with cols

// Sound mechanism details
const int soundSensorThresh = 339;
const int soundSensorRange = 100;
int soundSensorMax = soundSensorThresh + (soundSensorRange / 2);
int soundSensorMin = soundSensorThresh - (soundSensorRange / 2);

// Sound mechanism details - Bars
const int soundBarNeoPixelCount = 10;
int soundBarNeoPixelMin = 0;
int soundBarNeoPixelMax = soundBarNeoPixelCount - 1;
float soundBarNeoPixelMid = soundBarNeoPixelMax / 2.0;
boolean soundBarNeoPixelActive[soundBarNeoPixelCount];
int soundBarNeoPixelColorR[soundBarNeoPixelCount];
int soundBarNeoPixelColorG[soundBarNeoPixelCount];
int soundBarNeoPixelColorB[soundBarNeoPixelCount];

// sound bar active is used as follows:
//   0 - inactive
//   1 - active, bar goes up
//  -1 - active, bar goes down
int soundBarActive;
float soundBarRaw;
boolean soundBarPeakLoActive;
int soundBarPeakLo;
boolean soundBarPeakHiActive;
int soundBarPeakHi;
const int soundBarDropRate = 1;


// If this is true, debug mode is on
boolean stateDebug = true;


void setup() {
  // run once at beginning

  // prepare serial output
  Serial.begin(baud);
  Serial.println("Begin program");

  // initialize Circuit Playground
  CircuitPlayground.begin();

  // initialize NeoPixel Shield
  shield.begin();
  shield.show();  // Initialize all pixels to 'off'

  // setting initial values for variables
  previousMillisMatrix = 0;
  previousMillisRing = 0;
  previousMillisCPNeoPixel = 0;
  holdMatrix = 150;
  holdRing = 150;
  holdCPNeoPixel = 150;

  cpNeoPixel = 0;
  shieldX = 3;
  shieldY = 0;

  soundBarActive = 0;
  soundBarRaw = soundBarNeoPixelMid;
  soundBarPeakLoActive = false;
  soundBarPeakLo = soundBarNeoPixelMid;
  soundBarPeakHiActive = false;
  soundBarPeakHi = soundBarNeoPixelMid;
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


  // figure out sound bars
  soundBarRaw = float(soundBarNeoPixelMax - soundBarNeoPixelMin) / float(soundSensorMax - soundSensorMin) * float(stateSoundSensor - soundSensorMin) + float(soundBarNeoPixelMin);
  soundBarRaw = constrain(soundBarRaw, soundBarNeoPixelMin, soundBarNeoPixelMax);
  float soundBarActiveTest = (soundBarRaw - soundBarNeoPixelMid) * 10.0;
  if (abs(soundBarActiveTest) < 10) {
    soundBarActive = 0;
  } else {
    soundBarActive = abs(soundBarActiveTest) / soundBarActiveTest;
  }

  if (currentMillis - previousMillisCPNeoPixel > holdCPNeoPixel) {
    previousMillisCPNeoPixel = currentMillis;

    soundBarPeakLo += soundBarDropRate;
    if (soundBarPeakLo > soundBarNeoPixelMid) {
      soundBarPeakLo = soundBarNeoPixelMid;
      soundBarPeakLoActive = false;
    }

    soundBarPeakHi -= soundBarDropRate;
    if (soundBarPeakHi < soundBarNeoPixelMid) {
      soundBarPeakHi = soundBarNeoPixelMid;
      soundBarPeakHiActive = false;
    }
  }

  if (soundBarActive != 0) {
    if (soundBarActive > 0) {
      soundBarPeakHi = max(soundBarPeakHi, soundBarRaw);
      soundBarPeakHiActive = true;
    } else {
      soundBarPeakLo = min(soundBarPeakLo, soundBarRaw);
      soundBarPeakLoActive = true;
    }
  }

  for (int i = 0; i < soundBarNeoPixelCount; i++) {
    int pickColor = 0;

    if (soundBarPeakLoActive) {
      if ((i >= soundBarPeakLo) && (i < soundBarNeoPixelMid)) {
        pickColor = 1;
      }
    }
    if (soundBarPeakHiActive) {
      if ((i <= soundBarPeakHi) && (i > soundBarNeoPixelMid)) {
        pickColor = 1;
      }
    }

    if (soundBarActive != 0) {
      if (soundBarActive > 0) {
        if ((i <= soundBarRaw) && (i > soundBarNeoPixelMid)) {
          pickColor = 2;
        }
      } else {
        if ((i >= soundBarRaw) && (i < soundBarNeoPixelMid)) {
          pickColor = 2;
        }
      }
    }

    switch (pickColor) {
      case 1:  // red
        soundBarNeoPixelColorR[i] = 255;
        soundBarNeoPixelColorG[i] = 0;
        soundBarNeoPixelColorB[i] = 0;
        break;
      case 2:  // green
        soundBarNeoPixelColorR[i] = 0;
        soundBarNeoPixelColorG[i] = 255;
        soundBarNeoPixelColorB[i] = 0;
        break;
      default:  // black
        soundBarNeoPixelColorR[i] = 0;
        soundBarNeoPixelColorG[i] = 0;
        soundBarNeoPixelColorB[i] = 0;
        break;
    }
  }


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


  // debuging code for displays
  int pixel;
  if (stateDebug) {
    pixel = shieldPixel(shieldY, shieldX);
    shield.setPixelColor(pixel, 0, 0, 0);

    shieldX++;
    if(shieldX >= shieldCols) {
      shieldX = 0;
      shieldY++;
      if (shieldY >= shieldRows) {
        shieldY = 0;
      }
    }

    pixel = shieldPixel(shieldY, shieldX);
    shield.setPixelColor(pixel, 127, 127, 127);
    shield.show();
  }

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
    Serial.print("  peakLo: ");
    Serial.print(soundBarPeakLo);
    Serial.print("  peakHi: ");
    Serial.print(soundBarPeakHi);
    Serial.print("  active: ");
    Serial.println(soundBarActive);
    Serial.print("SHIELD rows: ");
    Serial.print(shieldRows);
    Serial.print("  cols: ");
    Serial.print(shieldCols);
    Serial.print("  Y: ");
    Serial.print(shieldY);
    Serial.print("  X: ");
    Serial.print(shieldX);
    Serial.print("  obj: ");
    Serial.println(pixel);

//    Serial.print("X: "); Serial.print(CircuitPlayground.motionX());
//    Serial.print(" \tY: "); Serial.print(CircuitPlayground.motionY());
//    Serial.print(" \tZ: "); Serial.print(CircuitPlayground.motionZ());
//    Serial.println(" m/s^2");

    delay(200);
  }
}


int shieldPixel(int row, int col) {
  // return the NeoPixel number for the desired row and col
  // remember that row 0 is first row, and col 0 is first col
  int pixel = (col * shieldRows) + row;
  return pixel;
}

