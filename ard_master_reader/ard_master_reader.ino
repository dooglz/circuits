// Wire Master Reader
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
}

const size_t bufLength = 3 * sizeof(int);

void loop() {
  Serial.print("---");
  Wire.requestFrom(8, bufLength);  //from slave device #8
  int out[3] = { -1, -1, -1};
  char* idx = (char*)(&out[0]);
  while (Wire.available() && idx < ((char*)(&out[0])+((char)bufLength))) { // slave may send less than requested
    char a = Wire.read();
    *idx = a; // receive a byte as character
    idx++;
  }
  Serial.print(out[0]); Serial.print(",");
  Serial.print(out[1]); Serial.print(",");
  Serial.print(out[2]);Serial.println("");
  delay(500);
}
