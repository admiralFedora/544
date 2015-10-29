#include <Average.h>

#define SAMPLES 9
#define PWPIN 7
#define OPIN 8 // output pin

//declare an array to store the samples. not necessary to zero the array values here, it just makes the code clearer
Average <int> avg(SAMPLES);
long pulse;
int modE; //AVERAGE DISTANCE


void setup() {
  //Open up a serial connection
  Serial.begin(9600);
  pinMode(PWPIN, INPUT);
  pinMode(OPIN, OUTPUT);
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
    digitalWrite(OPIN, LOW);
  }
  else 
  {
    digitalWrite(OPIN, HIGH);
  }
  delay(20);
}






