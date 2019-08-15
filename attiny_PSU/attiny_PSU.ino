
#include <Wire.h>
#define F_CPU 8000000
uint16_t pwm = 1;
uint16_t potinput = A2;
uint16_t feedbackinput = A3;
uint16_t VReq = 0;
uint16_t Vactual = 0;
uint16_t pwmval = 0;
float target = 0;
const uint16_t MAXPWM = 300;
uint16_t pwmLimit = MAXPWM;

uint16_t pwmSafetyRestTime = 0;

bool SafetyReduce = 0;
bool SafetyRise = 0;

uint16_t lastVal;

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
  Wire.onReceive(receiveEvent);
}

void requestEvent() {
  uint16_t out[5];
  out[0] = VReq;
  out[1] = Vactual;
  out[2] = (uint16_t)target;
  out[3] = pwmval;
  out[4] = pwmSafetyRestTime;
  Wire.write((char*)out, 5 * sizeof(uint16_t));
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
  // VReq = analogRead(potinput);
  // VReq = map(VReq, 1023, 0, 255, 0);
  Vactual = analogRead(feedbackinput);
  // Vactual = map(Vactual, 1023, 0, 255, 0);

  bool increasing = false;
  bool decreasing = false;


  //slowly move target towards VReq.
  if (Vactual < VReq) {
    target += 0.001f;
    increasing = true;
  } else if (Vactual > VReq) {
    target -= 0.01f;
    decreasing = true;
  }
  if (target < 0 ) {
    target = 0;
  }

  //Target can't get too far above of Vactual if increasing
  if (increasing && Vactual < target && (target - Vactual) < 5) {
    target = Vactual + 5;
  }

  //target can never be below Vactual if increasing
  if (increasing && Vactual > target) {
    target = Vactual;
  }

  //target can never be above of Vactual if decreasing
  if (decreasing && target > Vactual) {
    target = Vactual;
  }

  // Change pwmval based on difference between Vactual and target.
  if ((uint16_t)target > Vactual) {
    pwmval++;
  } else if ((uint16_t)target < Vactual && pwmval > 0) {
    pwmval--;
  }
/*
  if (!SafetyReduce) {
    pwmLimit = MAXPWM;
    //We need to increase a lot. But are not
    if (Vactual < target && target - Vactual > 10 && lastVal < target && Vactual <= lastVal) {
      pwmSafetyRestTime++;
    } else{
      pwmSafetyRestTime=0;
    }
    if (pwmSafetyRestTime > 100) {
      SafetyReduce = true;
      pwmSafetyRestTime = 1000;
    }
  } else {
    pwmLimit = 1;
    if (pwmSafetyRestTime > 0) {
      pwmSafetyRestTime--;
    } else {
      SafetyReduce = false;
    }
  }*/
  lastVal = Vactual;
  /*
    //if we hit limit (target always > Vactual)
    //progrssivly increase rest time every time this limit is hit
    if (pwmval >= pwmLimit && pwmSafetyRestTime < (MAXPWM +1)) {
      if (pwmSafetyRestTime == 0) {
        pwmSafetyRestTime = 3;
      } else {
        pwmSafetyRestTime *= 2 ;
      }
    }

    //reduce rest time by 1.
    if (pwmSafetyRestTime > 0) {
      pwmSafetyRestTime--;
     // pwmval = 0;
    }
    pwmLimit = MAXPWM - pwmSafetyRestTime;

    //awlays limit pwm
  */
  /*
    //Hit the limit, perhaps issue or mosfet thermal shutdown, reduce to protect
    if(pwmval >= pwmLimit && !SafetyReduce && !SafetyRise){
      SafetyReduce = true;
    }
    if(SafetyReduce){
      pwmLimit--;
    }
    if(SafetyReduce && pwmLimit < 5){
      SafetyReduce = false;
      SafetyRise = true;
    }
    if(SafetyRise && pwmLimit < MAXPWM){
      pwmLimit++;
    }else if(SafetyRise){
      SafetyRise = false;
    }
  */

  if (pwmval > pwmLimit) {
    pwmval = pwmLimit;
  }
  analogWrite(pwm, pwmval);

  /*
    while (VReq > Vactual) {
    if (pwmval == 230) {
      VReq = analogRead(potinput);
      VReq = map(VReq, 1023, 0, 255, 0);
      Vactual = analogRead(feedbackinput);
      Vactual = map(Vactual, 1023, 0, 255, 0);
    }
    else {
      pwmval = pwmval + 1;
      analogWrite(pwm, pwmval);
      VReq = analogRead(potinput);
      VReq = map(VReq, 1023, 0, 255, 0);
      Vactual = analogRead(feedbackinput);
      Vactual = map(Vactual, 1023, 0, 255, 0);
    }
     aled();
    }
    while (VReq < Vactual) {
    if (pwmval == 0) {
      VReq = analogRead(potinput);
      VReq = map(VReq, 1023, 0, 255, 0);
      Vactual = analogRead(feedbackinput);
      Vactual = map(Vactual, 1023, 0, 255, 0);
    }
    else {
      pwmval = pwmval - 1;
      analogWrite(pwm, pwmval);
      VReq = analogRead(potinput);
      VReq = map(VReq, 1023, 0, 255, 0);
      Vactual = analogRead(feedbackinput);
      Vactual = map(Vactual, 1023, 0, 255, 0);
    }
     aled();
    }*/
}
