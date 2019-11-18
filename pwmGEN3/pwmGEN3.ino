const int p1 = 2; //D2
const int p2 = 5; //D3
const int p3 = 7; //D2
const int p4 = 9; //D3

const int pins[4] = {p1,p2,p3,p4};
unsigned int delays[4] = {0,0,0,0};
String inputString = ""; 
bool stringComplete = false;  // whether the string is complete


unsigned long startMillis;  //some global variables available anywhere in the program
const unsigned long period = 1000;  //the value is a number of milliseconds

unsigned int i = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(pins[0], OUTPUT);
  pinMode(pins[1], OUTPUT);
  pinMode(pins[2], OUTPUT);
  pinMode(pins[3], OUTPUT);
  digitalWrite(pins[0], LOW);
  digitalWrite(pins[1], LOW);
  digitalWrite(pins[2], LOW);
  digitalWrite(pins[3], LOW);
  inputString.reserve(32);
  startMillis = millis();
  Serial.begin(9600);
  Serial.println("Hello");
}

void loop() {
  handleInput();
  unsigned long currentMillis = millis(); 
  if (currentMillis - startMillis >= period){
    handleChannels();
    startMillis = currentMillis;  
  }  
}

void handleChannels(){
  for (int chan = 0; chan < 4; chan++) {
    if(delays[chan]>0){
      delays[chan]--;
      digitalWrite(pins[chan], HIGH);
    }else{
      digitalWrite(pins[chan], LOW);
    }
  }
}

void handleInput(){
    while (Serial.available()) {
    if(i >=32){i=0;}//overflow protect
    i++;
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      i=0;
      stringComplete = true;
    }
  }
  if (stringComplete) {
    if(!validate()){
      Serial.println("bad");
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

bool validate(){
   if(inputString[0] == '!'){
      int sep = inputString.indexOf(',');
      if(sep != -1){
        int channel = inputString.substring(1,sep).toInt();
        int val = inputString.substring(sep+1).toInt();
        Serial.print(channel);Serial.print(",");
        Serial.println(val);
        if(channel >= 0 && channel < 4 && val >=0 && val <= 60){
          delays[channel] = val; 
          return true;
        }
      }
    }
    return false;
}
