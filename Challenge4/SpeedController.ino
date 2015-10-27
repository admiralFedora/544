/*
  Analog Input test for cyn70 - drc dec 2012
*/


//int pinRecoder = A2;    // select the input pin for the potentiometer
int sensorVal;  // variable to store the value coming from the sensor
//int sensorVal1;
volatile int count = 0;
int pin = 2;
int distance;
int perimeter = 10;

void setup() {
  Serial.begin (9600);
  pinMode(pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(pin), isr, CHANGE);
  //attachInterrupt(pin, isr, CHANGE);
}

void loop() {
  // read the value from the sensor:
  //sensorVal = analogRead(pinRecoder);
  //Serial.println(sensorVal);
  
  sensorVal = digitalRead(pin);
  Serial.println(sensorVal);
  delay(100);

  //delay(500);
  distance = perimeter*count/24;
  Serial.print("count:");
  Serial.println(count);
  count = 0;
}

void isr() {
    count++;
  }

