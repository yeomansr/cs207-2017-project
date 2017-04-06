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
 *  Connect Din on (nps) to #6 on (cp)
 *  
 *  Connect GND on (npr) to GND on (cp)
 *  Connect PWR +5V on (npr) to VBATT on (cp)
 *  Connect Data Input on (npr) to #3 on (cp)
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
const int pinShield = 6;
const int pinRing = 3;

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

// Adafruit NeoPixel Ring - 24 x 5050 RGB LED details
const int ringTotal = 24;
Adafruit_NeoPixel ring = Adafruit_NeoPixel(ringTotal, pinRing, NEO_GRB + NEO_KHZ800);
int ringX;

// Mode details
boolean mode1State;
boolean mode2State;

// Mode details - Dot
int dotDirection;
boolean dotDouble;

// Sound mechanism details
const int soundSensorThresh = 339;
const int soundSensorRange = 125;
int soundSensorMax = soundSensorThresh + (soundSensorRange / 2);
int soundSensorMin = soundSensorThresh - (soundSensorRange / 2);

// Sound mechanism details - Bars
const int soundBarNeoPixelCols = shieldCols;
float soundBarNeoPixelColRatio[] = { 0.45, 0.85, 1.0, 0.85, 0.45 };
const int soundBarNeoPixelCount = shieldRows;
int soundBarNeoPixelMin = 0;
int soundBarNeoPixelMax = soundBarNeoPixelCount - 1;
float soundBarNeoPixelMid = soundBarNeoPixelMax / 2.0;

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

// Accelerometer mechanism details
const float accelThresh = 0.5;
const float accelSensitivity = 0.03;
float accelX;  // + is force in direction towards power connector; - is force in direction towards microUSB connector
float accelY;  // + is force in direction towards pin #6 & #9; - is force in direction towards pin #0 & #2
float accelZ;  // + is force in direction below playground; - is force in direction above playground

// Accelerometer mechanism details - Dots
const int accelDotXColor = 0;
float accelDotX;
const int accelDotYColor = 85;
float accelDotY;
const int accelDotZColor = 171;
float accelDotZ;

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

  // initialize NeoPixel Ring
  ring.begin();
  ring.show();  // Initialize all pixels to 'off'

  // setting initial values for variables
  previousMillisShield = 0;
  previousMillisRing = 0;
  previousMillisCPNeoPixel = 0;
  holdShield = 300;
  holdRing = 50;
  holdCPNeoPixel = 75;

  cpNeoPixel = 0;
  cpNeoPixelColor = 0;
  shieldX = 3;
  shieldY = 0;
  ringX = 0;

  mode1State = false;
  mode2State = false;
  dotDirection = 1;
  dotDouble = false;

  soundBarActive = 0;
  soundBarRaw = soundBarNeoPixelMid;
  soundBarPeakLoActive = false;
  soundBarPeakLo = soundBarNeoPixelMid;
  soundBarPeakHiActive = false;
  soundBarPeakHi = soundBarNeoPixelMid;

  accelX = 0;
  accelY = 0;
  accelZ = 0;
  accelDotX = 0;
  accelDotY = 0;
  accelDotZ = 0;
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
  float stateMotionX = CircuitPlayground.motionX();
  float stateMotionY = CircuitPlayground.motionY();
  float stateMotionZ = CircuitPlayground.motionZ();

  // switch holds state, or releases state
  if (stateSwitch) {
    // check buttons
    if (stateButton1 != mode1State) {
      if (stateButton1) {
        // change direction of playground pattern
        dotDirection *= -1;
      }
    }
    if (stateButton2 != mode2State) {
      if (stateButton2) {
        // toggle double of playground pattern
        if (dotDouble) {
          dotDouble = false;
        } else {
          dotDouble = true;
        }
      }
    }

    // update mode states
    mode1State = stateButton1;
    mode2State = stateButton2;

    // run Circuit Playground NeoPixel reactor
    cpNeoPixelDisplay(currentMillis);

    // run sound reactor
    shieldDisplay(currentMillis, stateSoundSensor);

    // run accelerometer reactor
    ringDisplay(currentMillis, stateMotionX, stateMotionY, stateMotionZ);
  }


/*
  // debuging code for displays
  if (stateDebug) {
    int pixel = 0;

    if (stateSwitch) {
      pixel = 5;
    }

    CircuitPlayground.clearPixels();
    CircuitPlayground.setPixelColor(pixel, CircuitPlayground.colorWheel(0));
  }
*/

  // send message to serial output
  if (stateDebug) {
    // accelerometer details
    Serial.println();
    Serial.print("Switch: ");
    Serial.print(stateSwitch);
    Serial.print("  But 1: ");
    Serial.print(stateButton1);
    Serial.print("  But 2: ");
    Serial.println(stateButton2);

    delay(120);
  }
}


void cpNeoPixelDisplay(unsigned long currentMillis) {
  // update Circuit Playground NeoPixels

  // change NeoPixel to illuminate
  if (currentMillis - previousMillisCPNeoPixel > holdCPNeoPixel) {
    previousMillisCPNeoPixel = currentMillis;

    cpNeoPixel += dotDirection;
    if (cpNeoPixel < 0) {
      cpNeoPixel += cpNeoPixelCount;
    }
    if (cpNeoPixel >= cpNeoPixelCount) {
      cpNeoPixel -= cpNeoPixelCount;
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
  if (dotDouble) {
    CircuitPlayground.setPixelColor(cpNeoPixelCount - cpNeoPixel - 1, CircuitPlayground.colorWheel(cpNeoPixelColor));
  }
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
  soundBarRaw = constrain(soundBarRaw, soundBarNeoPixelMin - (soundBarNeoPixelCount / 2), soundBarNeoPixelMax + (soundBarNeoPixelCount / 2));

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
          soundBarNeoPixelColorR = 63;
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


void ringDisplay(unsigned long currentMillis, float valueX, float valueY, float valueZ) {
  // update ring display based on accelerometer values

  // slow down the movements
  if (currentMillis - previousMillisRing > holdRing) {
    previousMillisRing = currentMillis;

    // turn off dots
    ring.setPixelColor(accelX, 0, 0, 0);
    ring.setPixelColor(accelY, 0, 0, 0);
    ring.setPixelColor(accelZ, 0, 0, 0);

    // accelerometer values are greater than threshold, move dot
    if (abs(valueX) > accelThresh) {
      accelX += valueX * accelSensitivity;

      if (accelX < 0) {
        accelX += ringTotal;
      }
      if (accelX > ringTotal) {
        accelX -= ringTotal;
      }
    }
    if (abs(valueY) > accelThresh) {
      accelY += valueY * accelSensitivity;

      if (accelY < 0) {
        accelY += ringTotal;
      }
      if (accelY > ringTotal) {
        accelY -= ringTotal;
      }
    }
    if (abs(valueZ) > accelThresh) {
      accelZ += valueZ * accelSensitivity;

      if (accelZ < 0) {
        accelZ += ringTotal;
      }
      if (accelZ > ringTotal) {
        accelZ -= ringTotal;
      }
    }
  }

  // find integers from tracked values
  int dotX = accelX;
  int dotY = accelY;
  int dotZ = accelZ;

  // in case of overlapping, add colours together
  if (dotX == dotY) {
    if (dotX == dotZ) {
      ring.setPixelColor(dotX, 255, 255, 255);
    } else {
      ring.setPixelColor(dotX, 255, 0, 255);
      ring.setPixelColor(dotZ, 0, 255, 0);
    }
  } else {
    if (dotX == dotZ) {
      ring.setPixelColor(dotX, 255, 255, 0);
      ring.setPixelColor(dotY, 0, 0, 255);
    } else {
      if (dotY == dotZ) {
        ring.setPixelColor(dotX, 255, 0, 0);
        ring.setPixelColor(dotY, 0, 255, 255);
      } else {
        ring.setPixelColor(dotX, 255, 0, 0);
        ring.setPixelColor(dotY, 0, 0, 255);
        ring.setPixelColor(dotZ, 0, 255, 0);
      }
    }
  }

  // update ring
  ring.show();
}

