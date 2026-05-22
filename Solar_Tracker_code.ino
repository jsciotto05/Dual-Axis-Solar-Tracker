#include <Wire.h>
#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *elevationMotor = AFMS.getMotor(1);
Adafruit_StepperMotor *azimuthStepper = AFMS.getStepper(200, 2);

const int pinTL = A0;
const int pinTR = A1;
const int pinBL = A3;
const int pinBR = A2;

// Lower thresholds for indoor/flashlight use
const int horizThreshold = 15;
const int vertThreshold  = 15;

// Motion tuning
const int minStepSize    = 1;
const int maxStepSize    = 8;
const int stepperRPM     = 20;

const int minDcSpeed     = 110;
const int maxDcSpeed     = 200;
const int minDcPulseMs   = 10;
const int maxDcPulseMs   = 40;

const int loopDelayMs    = 15;
const int numSamples     = 4;

// Travel limiter
int elevationCount = 0;
const int elevationMinCount = -40;
const int elevationMaxCount = 40;

int readAveragedSensor(int pin) {
  long sum = 0;
  for (int i = 0; i < numSamples; i++) {
    sum += analogRead(pin);
  }
  return sum / numSamples;
}

int clampInt(int x, int xmin, int xmax) {
  if (x < xmin) return xmin;
  if (x > xmax) return xmax;
  return x;
}

void moveLeft(int steps) {
  azimuthStepper->step(steps, BACKWARD, SINGLE);
}

void moveRight(int steps) {
  azimuthStepper->step(steps, FORWARD, SINGLE);
}

void moveUp(int speedVal, int pulseMs) {
  if (elevationCount >= elevationMaxCount) {
    elevationMotor->run(RELEASE);
    return;
  }

  elevationMotor->setSpeed(speedVal);
  elevationMotor->run(FORWARD);
  delay(pulseMs);
  elevationMotor->run(RELEASE);
  elevationCount++;
}

void moveDown(int speedVal, int pulseMs) {
  if (elevationCount <= elevationMinCount) {
    elevationMotor->run(RELEASE);
    return;
  }

  elevationMotor->setSpeed(speedVal);
  elevationMotor->run(BACKWARD);
  delay(pulseMs);
  elevationMotor->run(RELEASE);
  elevationCount--;
}

void stopElevation() {
  elevationMotor->run(RELEASE);
}

void setup() {
  Serial.begin(9600);

  if (!AFMS.begin()) {
    Serial.println("Motor Shield not found");
    while (1);
  }

  azimuthStepper->setSpeed(stepperRPM);
  elevationMotor->run(RELEASE);

  Serial.println("2-axis solar tracker starting...");
}

void loop() {
  int TL = readAveragedSensor(pinTL);
  int TR = readAveragedSensor(pinTR);
  int BL = readAveragedSensor(pinBL);
  int BR = readAveragedSensor(pinBR);

  int leftAvg   = (TL + BL) / 2;
  int rightAvg  = (TR + BR) / 2;
  int topAvg    = (TL + TR) / 2;
  int bottomAvg = (BL + BR) / 2;

  int horizError = leftAvg - rightAvg;
  int vertError  = topAvg - bottomAvg;

  Serial.print("TL: "); Serial.print(TL);
  Serial.print(" TR: "); Serial.print(TR);
  Serial.print(" BL: "); Serial.print(BL);
  Serial.print(" BR: "); Serial.print(BR);
  Serial.print(" | HErr: "); Serial.print(horizError);
  Serial.print(" VErr: "); Serial.print(vertError);
  Serial.print(" ElevCount: "); Serial.println(elevationCount);

  // Horizontal proportional response
  if (abs(horizError) > horizThreshold) {
    int steps = map(abs(horizError), horizThreshold, 400, minStepSize, maxStepSize);
    steps = clampInt(steps, minStepSize, maxStepSize);

    if (horizError > 0) {
      moveLeft(steps);
    } else {
      moveRight(steps);
    }
  }

  // Vertical proportional response
  if (abs(vertError) > vertThreshold) {
    int speedVal = map(abs(vertError), vertThreshold, 400, minDcSpeed, maxDcSpeed);
    int pulseMs  = map(abs(vertError), vertThreshold, 400, minDcPulseMs, maxDcPulseMs);

    speedVal = clampInt(speedVal, minDcSpeed, maxDcSpeed);
    pulseMs  = clampInt(pulseMs, minDcPulseMs, maxDcPulseMs);

    if (vertError > 0) {
      moveUp(speedVal, pulseMs);
    } else {
      moveDown(speedVal, pulseMs);
    }
  } else {
    stopElevation();
  }

  delay(loopDelayMs);
}