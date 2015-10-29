#include <Average.h>

#define SAMPLES 9
#define PWPIN 7
#define OPIN 8 // output pin
#define SPEEDPIN 2 // speed controller pin

//declare an array to store the samples. not necessary to zero the array values here, it just makes the code clearer
Average <int> avg(SAMPLES);
long pulse;
int modE; //AVERAGE DISTANCE

int distance;
int perimeter = 59.69;//p=pi*19 cm
int sensorVal;  // variable to store the value coming from the sensor
volatile int countd = 0;

void setup() {
  //Open up a serial connection
  Serial.begin(9600);
  pinMode(PWPIN, INPUT);
  pinMode(OPIN, OUTPUT);
  pinMode(SPEEDPIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(SPEEDPIN), isr, CHANGE);
}

//Main loop where the action takes place
void loop() {

  for(int i = 0; i < SAMPLES; i++)
  {                    
    pulse = pulseIn(PWPIN, HIGH);
    avg.push(pulse/58);
    delay(10);
  }
  
  modE = avg.mode();
  //////////////////////////////////////////
  //COLLISION DETECTION
  //MINIMUM MEASURED DISTANCE 14 CENTIMETERS
  //MAXIMUM MEASURED DISTANCE 642 CENTIMETERS
  if(modE <= 20) //IF THE FRONT OF THE CAR GOT CLOSER THAN 50 CENTIMETERS, TERMINATE DRIVING
  {
    Serial.println("stopping");
    digitalWrite(OPIN, LOW);
  }
  else 
  {
    digitalWrite(OPIN, HIGH);
  }

  sensorVal = digitalRead(SPEEDPIN);
  distance = perimeter*countd/24;
  delay(20);
}

void isr() {
    countd++;
}





