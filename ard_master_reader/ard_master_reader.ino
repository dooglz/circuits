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
  Serial.print("Hello");
}

const size_t bufLength = 5 * sizeof(int);

void loop() {

  if (Serial.available() > 0) {
    // read the incoming byte:
    uint16_t val = (uint16_t)Serial.parseInt();
    // say what you got:
    Serial.print("I received: ");
    Serial.println(val, DEC);

    Wire.beginTransmission(8); // transmit to device

    byte myArray[2];
    myArray[0] = (val >> 8) & 0xFF;
    myArray[1] = val & 0xFF;
    Wire.write(myArray, 2);

    Wire.write((char*)&val, sizeof(uint16_t)); // sends x
    Wire.endTransmission(); // stop transmitting
  }

  Wire.requestFrom(8, bufLength);  //from slave device #8
  int out[5] = { -1, -1, -1, -1, -1};
  char* idx = (char*)(&out[0]);
  while (Wire.available() && idx < ((char*)(&out[0]) + ((char)bufLength))) { // slave may send less than requested
    char a = Wire.read();
    *idx = a; // receive a byte as character
    idx++;
  }
  const float VCC = 3.28;
  const float R1 = 4870.0;
  const float R2 = 327.0;
  const float Vout = (((VCC / 1023) * out[1]) * (R1 + R2)) / R2;
  Serial.print(out[0]); Serial.print(", ");
  Serial.print(out[1]); Serial.print(",");
  Serial.print(out[2]); Serial.print(", ");
  Serial.print(out[3]); Serial.print(",");
  Serial.print(out[4]); Serial.print(", (");
  Serial.print(Vout); Serial.println("V)");
  delay(1000);
}
