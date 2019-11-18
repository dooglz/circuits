const int p1 = 2; //D2
const int p2 = 5; //D3
const int p3 = 7; //D2
const int p4 = 9; //D3

const int pins[4] = {p1,p2,p3,p4};

const int de = 1000;
void setup() {
  // put your setup code here, to run once:
  pinMode(p1, OUTPUT);
  pinMode(p2, OUTPUT);
    pinMode(p3, OUTPUT);
      pinMode(p4, OUTPUT);
 //  digitalWrite(p1, LOW);
//  digitalWrite(p2, LOW);  
}

void loop() {
digitalWrite(pins[3], LOW);
digitalWrite(pins[0], HIGH);
delay(de);
digitalWrite(pins[0], LOW);
digitalWrite(pins[1], HIGH);
delay(de);
digitalWrite(pins[1], LOW);
digitalWrite(pins[2], HIGH);
delay(de);
digitalWrite(pins[2], LOW);
digitalWrite(pins[3], HIGH);
delay(de);
}
