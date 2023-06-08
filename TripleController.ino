#include <Motoron.h>
#include "Encoder.h"
#include "FSR.h"

// GLOBAL CONSTANTS
const uint16_t M3S256_ADDRESS = 17;  
const int BITS = 1023;
const int MAX_SPEED = 800;
const int LEFT_ARROW_KEY = 27;
const int RIGHT_ARROW_KEY = 26;
MotoronI2C mc; 
Encoder encoder(2, 3); // D2, D3 are interrupt pins on Uno
FSR fsr(A0, 30000);

// Global Function Prototypes
void print_vals(FSR fsr, Encoder e);
void motor_controller(int motor, FSR fsr1, FSR fsr2, Encoder e);
void dual_motor_controller(int home, const FSR& fsr, const Encoder& e, int m);
void keyboard_control(int motor);
// Generates Lambda Function for Interrupt Callback
typedef void (*InterruptHandler)();
InterruptHandler generateInterrupt(Encoder& e) {
  static Encoder* encoder = &e;
  
  // Define the lambda function as the callback
  auto callback = []() {
    encoder->read();
    if (encoder->ENCB() != encoder->ENCA()) {
      (*encoder)++;
    } else {
      (*encoder)--;
    }
  
  };
  // Convert the lambda function to a function pointer and return
  return callback;
}


void setup()
{
  Wire.begin();
  mc.reinitialize();    // Bytes: 0x96 0x74
  mc.disableCrc();      // Bytes: 0x8B 0x04 0x7B 0x43
  mc.clearResetFlag();  // Bytes: 0xA9 0x00 0x04

  // Configure motor 1
  mc.setMaxAcceleration(1, 140);
  mc.setMaxDeceleration(1, 300);
  encoder.setup(generateInterrupt(encoder));
  Serial.begin(9600);
}

//motor speed from -800 to +800
// using 9V Battery
void loop() {
  fsr.read();
  encoder.update();
  dual_motor_controller(5000, fsr, encoder, 1);
  dual_motor_controller(0, fsr, encoder, 2);

  print_vals(fsr, encoder);
}


void motor_controller(int motor, FSR fsr1, FSR fsr2, Encoder e) {
  const int LIMIT = 100;
  if (abs(fsr1.signal - fsr2.signal) <= 11) {
    if (e.ticks < -LIMIT) {
      mc.setSpeed(motor, 400);
    } else if (e.ticks > LIMIT) {
      mc.setSpeed(motor, -400);
    } else {
      mc.setSpeed(motor, 0);
    }
  } else if (fsr1.signal > fsr2.signal) {
    mc.setSpeed(motor, map(fsr1.signal, 0, 1023, 0, 800));    
  } else {
     mc.setSpeed(motor, -map(fsr2.signal, 0, 1023, 0, 800));    
  }
}

void keyboard_control(int motor) {
  if (Serial.available()) {
    char key = Serial.read();
    Serial.println(key);
  }
}


void dual_motor_controller(int home, const FSR& fsr, const Encoder& e, int m) {
  const int AUTO_LIMIT = 50;
  const int FWD_LIMIT = 10000; 

  if (fsr.signal <= 300) {
    if (e.ticks >  home + AUTO_LIMIT) {
      mc.setSpeed(m, -MAX_SPEED);
    } else {
      mc.setSpeed(m, 0);
    }
  } else {
    if (e.ticks < FWD_LIMIT) {
      mc.setSpeed(m, map(fsr.signal, 0, BITS, 0, MAX_SPEED));
    } else {
      mc.setSpeed(m, 0);
    }
  }
}

void print_vals(FSR fsr, Encoder e) {
  Serial.print(" "); 
  Serial.print(fsr.signal); 
  Serial.print(" "); 
  //Serial.print(fsr2.signal); 
  Serial.print(" ");
  Serial.print(e.ENCA());
  Serial.print(e.ENCB());
  Serial.print(" ");
  Serial.println(e.ticks);
}

