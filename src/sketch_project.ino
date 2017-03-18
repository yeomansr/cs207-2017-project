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
 *  2017-03-18
 * 
 * Name:
 *  Richard Yeomans
 * 
 */

// Include Adafruit libraries for operation of Circuit Playground
#include <Adafruit_CircuitPlayground.h>
#include <Wire.h>
#include <SPI.h>

// This is the baud rate to use when communicating with serial out
const int baud = 9600;

// This is the pin to control the NeoPixel Matrix
const int pinMatrix = 10;

// This is the pin to control the NeoPixel Ring
const int pinRing = 2;

// This is the motion delay for the NeoPixel Matrix
int holdMatrix;

// This is the motion delay for the NeoPixel Ring
int holdRing;

// This is the NeoPixel on the Circuit Playground we are tracking
int cpNeoPixel;

// This is the color for NeoPixel on the Circuit Playground we are tracking
int cpNeoPixelColor;

// This is the motion delay for the NeoPixel on the Circuit Playground
int holdCPNeoPixel;

// This is the stored time count for the NeoPixel Matrix
unsigned long previousMillisMatrix;

// This is the stored time count for the NeoPixel Ring
unsigned long previousMillisRing;

// This is the stored time count for the NeoPixel on the Circuit Playground
unsigned long previousMillisCPNeoPixel;

// This is the threshold for detecting sound on the sound sensor
int threshSoundSensor = 350;

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

  // send original message to serial output for reference
  Serial.println("Begin program");

  // initialize Circuit Playground
  CircuitPlayground.begin();

  // setting initial values for variables
  holdMatrix = 10;
  holdRing = 10;
  holdCPNeoPixel = 150;
  cpNeoPixel = 0;
  previousMillisMatrix = 0;
  previousMillisRing = 0;
  previousMillisCPNeoPixel = 0;
  historyCountSoundSensor = 0;
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

  // do stuff
  boolean stateCPNeoPixel;
  if (stateSoundSensor > threshSoundSensor) {
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

  // update all outputs
  if (stateCPNeoPixel) {
    for (int i = 0; i < 10; i++) {
      CircuitPlayground.setPixelColor(i, CircuitPlayground.colorWheel(cpNeoPixelColor));
    }
  } else {
    CircuitPlayground.clearPixels();
  }

  // send message to serial output
  if (stateDebug) {
    Serial.println(CircuitPlayground.soundSensor());
//    Serial.print("X: "); Serial.print(CircuitPlayground.motionX());
//    Serial.print(" \tY: "); Serial.print(CircuitPlayground.motionY());
//    Serial.print(" \tZ: "); Serial.print(CircuitPlayground.motionZ());
//    Serial.println(" m/s^2");
  }
}

