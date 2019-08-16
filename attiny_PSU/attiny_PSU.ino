
#include <Wire.h>
//#define F_CPU 8000000
const uint16_t pwm = 1;
const uint16_t feedbackinput = A3;

uint16_t VReq = 0;
uint16_t Vactual = 0;
uint16_t pwmval = 0;
float target = 0;

const uint16_t MAXPWM = 1000;

uint16_t lastVal;
uint16_t lastPWM;


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
  uint16_t out[4];
  out[0] = VReq;
  out[1] = Vactual;
  out[2] = (uint16_t)target;
  out[3] = pwmval;
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

  if (abs(target - VReq) < 1.0) {
    target = VReq;
  } else {
    if (VReq > target) {
      target += 0.1f;
    } else if (target > 0 && target > VReq) {
      target -=  0.2f;
    }
  }

  //close to Target, happily tickle pwm
  if (false && (float)Vactual / (float)target > 0.9 ) {
    if (target > Vactual) {
      pwmval++;
    } else if (pwmval > 0 &&  target < Vactual) {
      pwmval--;
    }
  } else {
    //Low voltage Headache Zone
    //Either a Lot to increase/decrease, or collapse
    if (Vactual < target) {
      //Need to increase a lot
      //Are we increasing?
      if (Vactual > lastVal) {
        //Yes, don't do anything
      } else {
        //Is PWM same/higher than last?
        if (pwmval >= lastPWM) {
          //if we are here, then an increase in P didn't increase V.
          if (pwmval > 53) {
            //We're probably too high. Reduce P double.
            if (pwmval > 2) {
              //no, reduce double.
              pwmval -= 2;
            }
          } else {
            //Probably Below Mimimum threshold
            pwmval++;
          }
        } else if (pwmval == lastPWM) {
          //no change in P, no increase in V. Try increasing P.
          pwmval++;
        } else if (pwmval < lastPWM) {
          //We Reduced P
          if (Vactual < lastVal) {
            //It reduced V, good?
            //But wrong direction
          } else {
            //assume V stayed the same, We're Detached above or below minimum V
            //Lower it some more anyway.
            if (pwmval > 1) {
              pwmval--;
            }
          }
        }
      }

    } else if (Vactual > target && pwmval > 0 ) {
      //Need to decrease a Lot
      pwmval--;
    }
  }

  lastVal = Vactual;
  lastPWM = pwmval;

  if (pwmval > MAXPWM) {
    pwmval = MAXPWM;
  }
  analogWrite(pwm, pwmval);

}
