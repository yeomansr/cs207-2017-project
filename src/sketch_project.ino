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
 * 
 * Legend:
 *  (cp)  - Adafruit Circuit Playground - Developer Edition
 *  (nps) - Adafruit NeoPixel Shield for Arduino - 40 RGB LED Pixel Matrix
 *  (npr) - Adafruit NeoPixel Ring - 24 x 5050 RGB LED
 * 
 * Setup:
 *  Connect Ext on (nps) to GND on (cp)
 *  Connect 5V on (nps) to VBATT on (cp)
 *  Connect Din on (nps) to #10 on (cp)
 *  
 *  Connect GND on (npr) to GND on (cp)
 *  Connect PWR +5V on (npr) to VBATT on (cp)
 *  Connect Data Input on (npr) to #2 on (cp)
 *  
 *  
 * Date:
 *  2017-04-02
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
const int pinShield = 10;
const int pinRing = 6;

// Stored time counts
unsigned long previousMillisCPNeoPixel;
unsigned long previousMillisShield;
unsigned long previousMillisRing;

// Motion delay thresholds
int holdCPNeoPixel;
int holdShield;
int holdRing;

// The NeoPixel on the Circuit Playground details
const int cpNeoPixelCount = 10;
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
const int soundSensorRange = 50;
int soundSensorMax = soundSensorThresh + (soundSensorRange / 2);
int soundSensorMin = soundSensorThresh - (soundSensorRange / 2);

// Sound mechanism details - Bars
const int soundBarNeoPixelCols = shieldCols;
float soundBarNeoPixelColRatio[] = { 0.6, 0.9, 1.0, 0.9, 0.6 };
const int soundBarNeoPixelCount = shieldRows;
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
float soundBarPeakLo;
boolean soundBarPeakHiActive;
float soundBarPeakHi;
const int soundBarDropRate = 1;


// If this is true, debug mode is on
boolean stateDebug = false;


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
  previousMillisShield = 0;
  previousMillisRing = 0;
  previousMillisCPNeoPixel = 0;
  holdShield = 300;
  holdRing = 150;
  holdCPNeoPixel = 75;

  cpNeoPixel = 0;
  cpNeoPixelColor = 0;
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

  // run Circuit Playground NeoPixel reactor
  cpNeoPixelDisplay(currentMillis);

  // run sound reactor
  shieldDisplay(currentMillis, stateSoundSensor);


/*
  // update all outputs
  for (int i = 0; i < soundBarNeoPixelCount; i++) {
    if (soundBarNeoPixelActive[i]) {
      CircuitPlayground.setPixelColor(i, soundBarNeoPixelColorR[i], soundBarNeoPixelColorG[i], soundBarNeoPixelColorB[i]);
    } else {
      CircuitPlayground.setPixelColor(i, soundBarNeoPixelColorR[i], soundBarNeoPixelColorG[i], soundBarNeoPixelColorB[i]);
    }
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

    delay(20);
  }
}


void cpNeoPixelDisplay(unsigned long currentMillis) {
  // update Circuit Playground NeoPixels

  // change NeoPixel to illuminate
  if (currentMillis - previousMillisCPNeoPixel > holdCPNeoPixel) {
    previousMillisCPNeoPixel = currentMillis;

    cpNeoPixel++;
    if (cpNeoPixel >= cpNeoPixelCount) {
      cpNeoPixel = 0;
    }
  }

  // change colour of NeoPixel
  cpNeoPixelColor++;
  if (cpNeoPixelColor >= 255) {
    cpNeoPixelColor = 0;
  }

  // update NeoPixels
  CircuitPlayground.clearPixels();
  CircuitPlayground.setPixelColor(cpNeoPixel, CircuitPlayground.colorWheel(cpNeoPixelColor));
}


int shieldPixel(int row, int col) {
  // return the NeoPixel number for the desired row and col
  // remember that row 0 is first row, and col 0 is first col
  int pixel = (col * shieldRows) + row;
  return pixel;
}

void shieldDisplay(unsigned long currentMillis, int valueRaw) {
  // update shield display based on sound sensor value

  // map raw value to sound bar raw value
  soundBarRaw = float(soundBarNeoPixelMax - soundBarNeoPixelMin) / float(soundSensorMax - soundSensorMin) * float(valueRaw - soundSensorMin) + float(soundBarNeoPixelMin);

  // if sound bar raw value is outside range, limit
  soundBarRaw = constrain(soundBarRaw, soundBarNeoPixelMin - soundBarNeoPixelCount, soundBarNeoPixelMax + soundBarNeoPixelCount);

  // decide if live bar should show something
  //  0 - no bar
  //  1 - bar above middle
  // -1 - bar below middle
  float soundBarActiveTest = (soundBarRaw - soundBarNeoPixelMid) * 10.0;
  if (abs(soundBarActiveTest) < 10) {
    soundBarActive = 0;
  } else {
    soundBarActive = abs(soundBarActiveTest) / soundBarActiveTest;
  }

  // slowly drop peak bars towards middle
  if (currentMillis - previousMillisShield > holdShield) {
    previousMillisShield = currentMillis;

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

  // if live bar is active, update appropriate peak bar, if necessary
  if (soundBarActive != 0) {
    if (soundBarActive > 0) {
      soundBarPeakHi = max(soundBarPeakHi, soundBarRaw);
      soundBarPeakHiActive = true;
    } else {
      soundBarPeakLo = min(soundBarPeakLo, soundBarRaw);
      soundBarPeakLoActive = true;
    }
  }

  // draw bars on shield
  for (int i = 0; i < soundBarNeoPixelCols; i++) {
    float ratio = soundBarNeoPixelColRatio[i];
    float ratioPeakLo = (soundBarPeakLo - soundBarNeoPixelMid) * ratio + soundBarNeoPixelMid;
    float ratioPeakHi = (soundBarPeakHi - soundBarNeoPixelMid) * ratio + soundBarNeoPixelMid;
    float ratioRaw = (soundBarRaw - soundBarNeoPixelMid) * ratio + soundBarNeoPixelMid;

    for (int j = 0; j < soundBarNeoPixelCount; j++) {
      // set default colour to black
      int pickColor = 0;

      // if within peak bar ranges, change to peak colour
      if (soundBarPeakLoActive) {
        if ((j >= ratioPeakLo) && (j < soundBarNeoPixelMid)) {
          pickColor = 1;
        }
      }
      if (soundBarPeakHiActive) {
        if ((j <= ratioPeakHi) && (j > soundBarNeoPixelMid)) {
          pickColor = 1;
        }
      }

      // if within live bar range, change to live colour
      if (soundBarActive != 0) {
        if (soundBarActive > 0) {
          if ((j <= ratioRaw) && (j > soundBarNeoPixelMid)) {
            pickColor = 2;
          }
        } else {
          if ((j >= ratioRaw) && (j < soundBarNeoPixelMid)) {
            pickColor = 2;
          }
        }
      }

      int soundBarNeoPixelColorR;
      int soundBarNeoPixelColorG;
      int soundBarNeoPixelColorB;
      switch (pickColor) {
        case 1:  // red
          soundBarNeoPixelColorR = 255;
          soundBarNeoPixelColorG = 0;
          soundBarNeoPixelColorB = 0;
          break;
        case 2:  // green
          soundBarNeoPixelColorR = 0;
          soundBarNeoPixelColorG = 255;
          soundBarNeoPixelColorB = 0;
          break;
        default:  // black
          soundBarNeoPixelColorR = 0;
          soundBarNeoPixelColorG = 0;
          soundBarNeoPixelColorB = 0;
          break;
      }

      // update NeoPixel colour (does not update shield immediately)
      int pixel = shieldPixel(j, i);
      shield.setPixelColor(pixel, soundBarNeoPixelColorR, soundBarNeoPixelColorG, soundBarNeoPixelColorB);
    }
  }

  // update shield
  shield.show();
}

