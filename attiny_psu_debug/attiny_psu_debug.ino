
#include <Wire.h>
//#define F_CPU 8000000
const uint16_t pwm = 1;
const uint16_t feedbackinput = A3;

uint16_t VReq = 0;
uint16_t Vactual = 0;
const uint16_t MAXPWM = 255;

const uint16_t THERMMAX = 700;
const uint16_t THERMSLOW = 610;

uint16_t lastVal;
uint16_t lastPWM = 0;

uint16_t tempval = 0;

#define pin5 PIN_B0
#define pin7 PIN_B2
#define tempPin A2

/* 1-v
   2-7
   3-6
   G-5
*/

void setup() {
  TCCR0A = 2 << COM0A0 | 2 << COM0B0 | 3 << WGM00;
  TCCR0B = 0 << WGM02 | 1 << CS00;
  TCCR1 = 0 << PWM1A | 0 << COM1A0 | 1 << CS10;
  GTCCR = 1 << PWM1B | 2 << COM1B0;
  pinMode(pwm, OUTPUT);
  pinMode(feedbackinput, INPUT);
  pinMode(tempPin, INPUT);
  digitalWrite(pwm, LOW);
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent);
}

void requestEvent() {
  uint16_t out[4];
  out[0] = VReq;
  out[1] = Vactual;
  out[2] = tempval;
  out[3] = lastPWM;
  Wire.write((char*)out, 4 * sizeof(uint16_t));
}

void receiveEvent(size_t numBytes) {
  uint16_t newT = 0;
  byte a, b;
  a = Wire.read();
  b = Wire.read();
  newT = a;
  newT = newT << 8 | b;
  if (newT >= 0 && newT < 1023) {
    VReq = newT;
  }
}

void loop() {
  Vactual = analogRead(feedbackinput);
  tempval = analogRead(tempPin);

  uint16_t pwmval = VReq;
  if(tempval >= THERMMAX){
    pwmval = 0;
  }else if(tempval > THERMSLOW){
    uint16_t a = (tempval - THERMSLOW);
    if(a <= pwmval){
      pwmval -= a;
     }else{
      pwmval = 0;
     }
  }
  
  lastPWM = pwmval;
  analogWrite(pwm, pwmval);
}
