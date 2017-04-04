/*
  Ball shooter

 This class contains the control of the system specified in the documents produced 
 by group 33 in the course 4CC10

 created 21 Mar 2017
 modified 31 May 2012
 by Cameron Weibel

 */

// Included libraries
#include <Stepper.h>    // included in IDE
 
// Constants
const int VERBOSE = 1;    // enables print statements 
const int PRESENCE_THRESHOLD = 400    // distance threshold for ball presence 

// IR globals
int sensorIR1 = 3;    // analog pin used to connect the 1st IR sensor
int valIR1 = 0;    // variable to store the values from sensor(initially zero)

// Velocity globals
int sensorIR2 = 5;    // analog pin used to connect the 2nd IR sensor
int sensorIR3 = 6;    // analog pin used to connect the 3rd IR sensor

unsigned long start = 0;    //  timer for IR sensor 2
unsigned long finish = 0;    // timer for IR sensor 3

int distance = 10;    // fixed distances between IR sensor 2 and 3
int done = 0;    // boolean if readings are done

// Stepper motor globals
int in1Pin = 12;
int in2Pin = 11;
int in3Pin = 10;
int in4Pin = 9;

int stepsTo45 = 100;    // number of steps required by motor to move barrel to/from 45 deg

Stepper motor(512, in1Pin, in2Pin, in3Pin, in4Pin);    // init motor
 
// EM linear actuator globals
int forcePin = 13

int runs = 0;    // total number of successful cycles

// Setup run once every Arduino boot
void setup() {
  // initialize serial communications (for debugging only):
  Serial.begin(9600);
  // set motor speed
  motor.setSpeed(20);
  // init digital pins
  pinMode(forcePin);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
}

// Continuous looping of shooting behavior
void loop() {
  // wait for ball to be loaded
  if (VERBOSE) Serial.println("Awaiting ball...");
  awaitBall();
  
  // elevate barrel
  if (VERBOSE) Serial.println("Elevating barrel...");
  elevateBarrel();
  
  // shoot ball
  if (VERBOSE) Serial.println("Shooting ball...");
  shootBall();
  
  // lower barrel
  if (VERBOSE) Serial.println("Lowering barrel...");
  lowerBarrel();
  
  // increment and print total number of balls shot
  runs++;
  if (VERBOSE) Serial.println("Number of completed runs: " + runs);

  // delay for stability, (can remove)
  delay(500);
}

// Waits for ball to be present before completing
void awaitBall() {
    // Loop until ball is detected
    while (valIR1 < PRESENCE_THRESHOLD) {
        valIR1 = analogRead(sensorIR1);    // reads the value of the IR sensor
        Serial.println(valIR1);    // prints the value of the sensor to the serial monitor
        delay(400);    // wait for this much time before printing next value
    }
    if (VERBOSE) Serial.println("Ball detected");
    valIR1 = 0;    // reset sensor reading
}

// Elevates barrel of projectiler
void elevateBarrel() {
    motor.step(stepsTo45);    // step to 45 deg
    delay(500);    // may need to delay while motor is stepping
    if (VERBOSE) Serial.println("Barrel elevated");
}

// Shoots ball
void shootBall() {
    digitalWrite(forcePin, HIGH);    // fire EM actuator
    delay(500);    // delay between switching
    digitalWrite(forcePin, LOW);     // stop actuation
    if (VERBOSE) Serial.println("Ball shot");
}

// Lowers barrel of projectiler
void lowerBarrel() {
    motor.step(-stepsTo45);
    delay(500);    // may need to delay while motor is stepping
    if (VERBOSE) Serial.println("Barrel elevated");
}

// Calculates velocity of projectile using two IR sensors placed at a fixed distance within the barrel
void calcVelocity() {
  // Wait for first sensor to be triggered
  while(!start) {
    if(analogRead(sensorIR2) > PRESENCE_THRESHOLD) start = micros();
  }
  // Wait for second sensor to be triggered
  while(!finish) {
    if(analogRead(sensorIR3) > PRESENCE_THRESHOLD) finish = micros();
  }
  // Caulcated lates velocity and exits
  while(!done) {
    interval = float(finish-start);
    velocity = distance*1000*3.28/interval;
    if (VERBOSE) Serial.println("Velocity in ft/sec: " + velocity);
    done = 1;
  }
  done = 0;    // reset done flag
}