#include <Motoron.h>
#include <EEPROM.h>
#include "Encoder.h"
#include "FSR.h"

// GLOBAL CONSTANTS
const int M3S256_ADDRESS = 17;  
const int EEPROM_ADDRESS = 0;

const int BITS = 1023;
const int MAX_SPEED = 800;

MotoronI2C mc; 
Encoder encoder(2, 3); // D2, D3 are interrupt pins on Uno
FSR fsr(A0, 30000);

// Global Function Prototypes
void print_vals(FSR fsr, Encoder e);
void reset_eeprom(int address, long data=0);

void dual_control(int motor, FSR fsr1, FSR fsr2, Encoder e);
void single_control(int home, const FSR& fsr, const Encoder& e, int m);
void keyboard_control(int m, const Encoder& e, int tks);


// Generates Lambda Function for Interrupt Callback
typedef void (*InterruptHandler)();
InterruptHandler generateInterrupt(Encoder& e) {
  static Encoder* encoder = &e;
  auto callback = []() {
    encoder->read();
    if (encoder->ENCB() != encoder->ENCA()) {
      (*encoder)++;
    } else {
      (*encoder)--;
    }
  };
  return callback;
}


void setup()
{
  Serial.begin(9600);

  Wire.begin();
  mc.reinitialize();    // Bytes: 0x96 0x74
  mc.disableCrc();      // Bytes: 0x8B 0x04 0x7B 0x43
  mc.clearResetFlag();  // Bytes: 0xA9 0x00 0x04

  // Configure motors
  mc.setMaxAcceleration(1, 140);
  mc.setMaxDeceleration(1, 300);
  mc.setMaxAcceleration(2, 140);
  mc.setMaxDeceleration(2, 300);
  
  //Read 4 bytes (long) from eeprom memory to initialize encoder position

  long eeprom;
  EEPROM.get(EEPROM_ADDRESS, eeprom);  
  encoder.setup(eeprom, generateInterrupt(encoder));
}

//motor speed from -800 to +800
// using 9V Battery
void loop() {
  Serial.println(encoder.ticks);
  fsr.read();
  encoder.update();
  if (Serial.available()) {
    int tks = Serial.parseInt();
    Serial.println(tks);
    //Serial.print(" ");
    keyboard_control(1, encoder, tks); //turns motor 1
    keyboard_control(2, encoder, tks); //turns motor 2
    //Serial.println(encoder.ticks);
  }
   
  //print_vals(fsr, encoder);
  EEPROM.put(EEPROM_ADDRESS, encoder.ticks); //update eeprom memory with current motor location
}

/*
  Uses 2 FSR's to control the backword and forwoard motion of the motors seperatly.
  The speed of the motor is proportional to the FSR signal.
*/
void dual_control(int motor, FSR fsr1, FSR fsr2, Encoder e) {
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

/*
  Uses the user input through the Serial port to control
  absolute location of the motor. Make sure that the
  Serial Moniter sends data in "no new line ending" mode
*/
void keyboard_control(int m, const Encoder& e, int tks) {
  const uint32_t LIM = 5;
  if (tks < e.ticks) {
    while (tks < e.ticks) {
      mc.setSpeed(m, -400);
    }
    mc.setSpeed(m, 0);
  } else if (tks > e.ticks) {
      while (tks < e.ticks) {
        mc.setSpeed(m, 400);
      }
      mc.setSpeed(m, 0);
  } else {
    mc.setSpeed(m, 0);
  }
  Serial.println(e.ticks);
}

/*
  Control scheme where only 1 FSR is used to control the motors.
  The motors are driven forward while a signal is detected from the FSR
  and automatically driven back to the home position when no FSR signal
  is detected. Speed of forward drive is proportional to FSR signal
*/
void single_control(int home, const FSR& fsr, const Encoder& e, int m) {
  const int AUTO_LIMIT = 50;
  const int FWD_LIMIT = 10000; 

  if (fsr.signal <= 700) {
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

/*
  Prints values of FSR and Encoder objects to Serial port
*/
void print_vals(FSR fsr, Encoder e) {
  Serial.print(" "); 
  Serial.print(fsr.signal); 
  Serial.print(" "); 
  Serial.print(" ");
  Serial.print(e.ENCA());
  Serial.print(e.ENCB());
  Serial.print(" ");
  Serial.println(e.ticks);
}

/*
  Clears the first 4 bytes (long) of EEPROM memory back to zero
*/
void reset_eeprom(int address, long data=0) {
  EEPROM.put(address, data);
}
