#include "Encoder.h"

Encoder::Encoder(int pin_a, int pin_b) {
  pinA = pin_a;
  pinB = pin_b;
  a = digitalRead(pinA);
  b = digitalRead(pinB);
  ticks = 0;
  time = 0;
}

int Encoder::ENCA() {
  return a;
}

int Encoder::ENCB() {
  return b;
}

void Encoder::setPins(int pin_a, int pin_b) {
  pinA = pin_a;
  pinB = pin_b;
}

void Encoder::read() {
  a = digitalRead(pinA);
  b = digitalRead(pinB);
}

//26 x 298
void Encoder::update() {
  pulses = ticks;
  time = micros()/1e06;
}

void Encoder::print() {
  Serial.print(" ");
  Serial.print(a);
  Serial.println(b);
}

void Encoder::setup(void (*function)()) {
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  attachInterrupt(digitalPinToInterrupt(pinA), function, CHANGE);
}

double Encoder::position(float gearing) {
  return ticks / TICKS_PER_REV / gearing;
}

Encoder& Encoder::operator++(int) {
  ticks++;
  return *this;
}

Encoder& Encoder::operator--(int) {
  ticks--;
  return *this;
}

double Encoder::rpm(float gearing) {
  return (ticks - pulses) / (micros()/1e06 - time) / TICKS_PER_REV / gearing;
}
