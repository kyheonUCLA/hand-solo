#include <Motoron.h>
#include <EEPROM.h>
#include "Encoder.h"
#include "FSR.h"

// GLOBAL CONSTANTS
const int M3S256_ADDRESS = 17;  
const int EEPROM_ADDRESS = 0;

const int BITS = 1023;
const int MAX_SPEED = 800;

int currentTicks = 0;
long eeprom;

MotoronI2C mc; 
Encoder encoder(2, 3); // D2, D3 are interrupt pins on Uno
FSR fsr(A0, 30000);

// Global Function Prototypes
void print_vals(FSR fsr, Encoder e);
void getKeyboardInput(int& ticks);

void dual_control(int motor, FSR fsr1, FSR fsr2, Encoder e);
void single_control(int home, const FSR& fsr, const Encoder& e, int m);
void keyboard_control(const Encoder& e, int target);


// Generates Lambda Function for Interrupt Callback
typedef void (*InterruptHandler)();
InterruptHandler generateInterrupt(Encoder& e) 
{
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
  //EEPROM.put(EEPROM_ADDRESS, (long)0)
  EEPROM.get(EEPROM_ADDRESS, eeprom);  
  encoder.setup(eeprom, generateInterrupt(encoder));
  currentTicks = eeprom;
}


//motor speed from -800 to +800
// using 9V Battery
void loop() {
  // fsr.read();
  encoder.update();
  
  getKeyboardInput(currentTicks);
  keyboard_control(encoder, currentTicks);
 
  //print_vals(fsr, encoder);
  EEPROM.put(EEPROM_ADDRESS, encoder.ticks); //update eeprom motor location
  Serial.println(" ");
  encoder.print();
  //delay(500);
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
void keyboard_control(const Encoder& e, int target) {
  const long LIM = 50;
  if (e.ticks < target - LIM) {
    mc.setSpeed(3, 800);
    mc.setSpeed(2, 800);
    Serial.println(" ");
    encoder.print();
  } else if (e.ticks > target + LIM) {
    mc.setSpeed(3, -800);
    mc.setSpeed(2, -800);
    Serial.println(" ");
    encoder.print();
  } else {
    mc.setSpeed(3, 0);
    mc.setSpeed(2, 0);
  }
}


/*
  Control scheme where only 1 FSR is used to control the motors.
  The motors are driven forward while a signal is detected from the FSR
  and automatically driven back to the home position when no FSR signal
  is detected. Speed of forward drive is proportional to FSR signal
*/
void single_control(int home, const FSR& fsr, const Encoder& e) {
  const int AUTO_LIMIT = 50;
  const int FWD_LIMIT = 10000; 
  const int DETECTION_THRESHOLD = 700;
  
  if (fsr.signal <= DETECTION_THRESHOLD) {
    if (e.ticks >  home + AUTO_LIMIT) {
      mc.setSpeed(1, -MAX_SPEED);
      mc.setSpeed(2, -MAX_SPEED);
    } else {
      mc.setSpeed(1, 0);
      mc.setSpeed(2, 0);
    }
  } else {
    if (e.ticks < FWD_LIMIT) {
      mc.setSpeed(1, map(fsr.signal, 0, BITS, 0, MAX_SPEED));
      mc.setSpeed(2, map(fsr.signal, 0, BITS, 0, MAX_SPEED));
    } else {
      mc.setSpeed(1, 0);
      mc.setSpeed(2, 0);
    }
  }
}

void getKeyboardInput(int& ticks) {
  if (Serial.available()) {
    ticks = Serial.parseInt();
    Serial.println(ticks);   
  }  
}

/*
  Prints values of FSR and Encoder objects to Serial port
*/
void print_vals(FSR fsr, Encoder e) {
  Serial.println(" "); 
  fsr.print();
  Serial.print(" "); 
  Serial.print(" ");
  e.print();
}

