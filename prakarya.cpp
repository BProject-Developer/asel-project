#include <Servo.h>

Servo doorServo;

const int PIR_PIN = 2;
const int SERVO_PIN = 9;

const int openAngle  = 90;
const int closeAngle = 0;
const unsigned long closeDelay = 5000;

bool isOpen = false;
unsigned long lastMotionMillis = 0;

void setup() {
  pinMode(PIR_PIN, INPUT);
  doorServo.attach(SERVO_PIN);
  doorServo.write(closeAngle);
  delay(500); 
  doorServo.detach(); 

void loop() {
  int pirState = digitalRead(PIR_PIN);

  if (pirState == HIGH) {
    lastMotionMillis = millis();
    if (!isOpen) {
      doorServo.attach(SERVO_PIN);
      doorServo.write(openAngle);
      delay(500); 
      doorServo.detach();
      isOpen = true;
    }
  }

  if (isOpen && millis() - lastMotionMillis > closeDelay) {
    doorServo.attach(SERVO_PIN);
    doorServo.write(closeAngle);
    delay(500);
    doorServo.detach();
    isOpen = false;
  }
}