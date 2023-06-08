#include <Arduino.h>

#ifndef FSR_H
#define FSR_H

class FSR {
private:
  int pin;
  int rm;
  double V2F(double v, const double V[], const double F[], int n);
public:
  double signal;
  FSR(int newPin, int rm);
  void read();
  void setPin(int newPin);
  double force();
  void print();
  void printSignal();

  bool operator<(const FSR& right) const;
  bool operator>(const FSR& right) const;
  bool operator==(const FSR& right) const;
  bool operator!=(const FSR& right) const;
  bool operator<=(const FSR& right) const;
  bool operator>=(const FSR& right) const;
};

#endif
