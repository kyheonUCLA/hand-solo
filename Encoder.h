#include <Arduino.h>

#ifndef ENCODER_H
#define ENCODER_H

class Encoder {
public:
  int pinA;
  int pinB;
  int a;
  int b;
  volatile long ticks;
  long pulses;
  double time;
public:
  const float TICKS_PER_REV = 26.0;
  Encoder(int pin_a, int pin_b);
  int ENCA();
  int ENCB();
  void read();
  void update();
  void print();
  void setPins(int pin_a, int pin_b);
  void setup(long eeprom, void (*function)());
  double position(float gearing);
  double rpm(float gearing);
  Encoder& operator++(int);
  Encoder& operator--(int);
};

#endif
