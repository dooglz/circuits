
#include <Wire.h>
#define F_CPU 8000000
int pwm = 1;
int potinput = A2;
int feedbackinput = A3;
int potinputval;
int feedbackinputval;
int pwmval;

#define pin5 PIN_B0
#define pin7 PIN_B2

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
  pinMode(potinput, INPUT);
  pinMode(feedbackinput, INPUT);
  
 // pinMode(pin5, OUTPUT);
 // pinMode(pin7, OUTPUT);
  
  digitalWrite(pwm, LOW);
  pwmval = 0;

  Wire.begin(8);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
}

void requestEvent() {
  int out[3];
  out[0] = potinputval;
  out[1] = feedbackinputval;
  out[2] = pwmval;
  Wire.write((char*)out,3*sizeof(int)); // respond with message of 6 bytes
  // as expected by master
}

void aled(){
    if(potinputval > feedbackinputval){
   // digitalWrite(pin7, LOW);
   // digitalWrite(pin5, HIGH);
  }
  if(potinputval < feedbackinputval){
  //  digitalWrite(pin7, HIGH);
   // digitalWrite(pin5, LOW);
  }
}

void loop() {
  potinputval = analogRead(potinput);
  potinputval = map(potinputval, 1023, 0, 255, 0);
  feedbackinputval = analogRead(feedbackinput);
  feedbackinputval = map(feedbackinputval, 1023, 0, 255, 0);


  aled();
  while (potinputval > feedbackinputval) {
    if (pwmval == 230) {
      potinputval = analogRead(potinput);
      potinputval = map(potinputval, 1023, 0, 255, 0);
      feedbackinputval = analogRead(feedbackinput);
      feedbackinputval = map(feedbackinputval, 1023, 0, 255, 0);
    }
    else {
      pwmval = pwmval + 1;
      analogWrite(pwm, pwmval);
      potinputval = analogRead(potinput);
      potinputval = map(potinputval, 1023, 0, 255, 0);
      feedbackinputval = analogRead(feedbackinput);
      feedbackinputval = map(feedbackinputval, 1023, 0, 255, 0);
    }
     aled();
  }
  while (potinputval < feedbackinputval) {
    if (pwmval == 0) {
      potinputval = analogRead(potinput);
      potinputval = map(potinputval, 1023, 0, 255, 0);
      feedbackinputval = analogRead(feedbackinput);
      feedbackinputval = map(feedbackinputval, 1023, 0, 255, 0);
    }
    else {
      pwmval = pwmval - 1;
      analogWrite(pwm, pwmval);
      potinputval = analogRead(potinput);
      potinputval = map(potinputval, 1023, 0, 255, 0);
      feedbackinputval = analogRead(feedbackinput);
      feedbackinputval = map(feedbackinputval, 1023, 0, 255, 0);
    }
     aled();
  }
}
