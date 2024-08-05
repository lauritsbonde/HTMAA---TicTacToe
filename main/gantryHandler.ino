#include <Servo.h>

/*
 Stepper Motor Control -  revolution

 This program drives a unipolar or bipolar stepper motor.
 The motor is attached to digital pins 8 - 11 of the Arduino.

 The motor should revolve one revolution in one direction, then
 one revolution in the other direction.


 */
Servo Servo1; 
int servoPin = D9;

const int electroMagnet = D5;

const int stepX_pin = D3;
const int dirX_pin = D8;
const int homeX_pin = D12;

const int stepY_pin = D4;
const int dirY_pin = D7;
const int homeY_pin = D13;

const int steps = 380;

void homeMotors(){
  Serial.println("Starting home");
  digitalWrite(dirX_pin, HIGH);
  while(digitalRead(homeX_pin) != 1){
    digitalWrite(stepX_pin, HIGH);
    delayMicroseconds(50);
    digitalWrite(stepX_pin, LOW);
    delayMicroseconds(1000);
  }

  Serial.println("home1");

  digitalWrite(dirY_pin, LOW);
  while(digitalRead(homeY_pin) != 1){
    digitalWrite(stepY_pin, HIGH);
    delayMicroseconds(50);
    digitalWrite(stepY_pin, LOW);
    delayMicroseconds(1000);
  }
  // Put the servo down
  goDown(LOW);
  Serial.println("Motors are home!");
}

void setupGantry() {
  pinMode(electroMagnet, OUTPUT);

  pinMode(stepX_pin, OUTPUT);
  pinMode(dirX_pin, OUTPUT);
  pinMode(homeX_pin, INPUT);

  pinMode(stepY_pin, OUTPUT);
  pinMode(dirY_pin, OUTPUT);
  pinMode(homeY_pin, INPUT);

  Servo1.attach(servoPin, 500, 2400);

  homeMotors();
  delay(1000);
}

void goUp(int magnetStatus){
  for(int i = 180; i>= 0; i-= 1) {
    Servo1.write(i);
    delay(15);
  }
  // Turn of magnet
  digitalWrite(electroMagnet, magnetStatus);
}

void goDown(int magnetStatus){
  for(int i = 0; i<= 180; i+= 1) {
    Servo1.write(i);
    delay(15);
  }
  // Turn on magnet
  digitalWrite(electroMagnet, magnetStatus);
}

// it goes row,col, from bottom left to top right
int gamePlaces[11][2] = {
  {0,0}, // pickup computer piece
  {3,0}, // pickup human piece - should not be used
  {0,1},
  {0,2},
  {0,3},
  {1,1},
  {1,2},
  {1,3},
  {2,1},
  {2,2},
  {2,3},
};

void moveX(int dir, int steps) {
  digitalWrite(dirX_pin, dir);
  digitalWrite(stepX_pin, LOW);

  for (int i = 0; i < steps; i++) {
    digitalWrite(stepX_pin, HIGH);
    delayMicroseconds(50);
    digitalWrite(stepX_pin, LOW);
    delayMicroseconds(1000);
  }
}

void moveY(int dir, int steps) {
  digitalWrite(dirY_pin, dir);
  digitalWrite(stepY_pin, LOW);

  for(int i = 0; i < steps; i++){
    digitalWrite(stepY_pin, HIGH);
    delayMicroseconds(50);
    digitalWrite(stepY_pin, LOW);
    delayMicroseconds(1000);
  }
}

// we assume the gantry starts from "home"
void moveTo(int row, int col) {
  moveX(LOW, steps * row);
  moveY(HIGH, steps * col);
}

void pickUp(){
  goDown(HIGH);
  delay(50);
  goUp(HIGH);
}

void putDown(){
  goDown(LOW);
  delay(50);
  goUp(LOW);
}

void placePiece(int row, int col) {
  pickUp();
  moveTo(row, col);
  putDown();
}
