#include <SoftwareSerial.h>
#include <ArduinoJson.h>
SoftwareSerial XBee(2, 3); // Rx, Tx

#define PIN A0
#define VCC 3.3
#define SERIESRESISTOR 10000
#define SAMPLES 10
#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 25
#define BCOEFFICIENT 3950
#define ID 2       //Change here to identify each Sensor

void sendJson(float reading){
  char buffer[100];
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["id"] = ID;
  root["temp"] = reading;

  root.printTo(buffer, sizeof(buffer));
  XBee.println(buffer);
  Serial.println(buffer);
}

float getAverage(){
  int i;
  float average;
  average = 0;
  for(i = 0; i < SAMPLES; i++){
    average += analogRead(PIN);
    delay(10);
  }
  average /= SAMPLES;
  return ((1023 / average) - 1);
}

float steinhart(float resistance){
  float steinhart;
  steinhart = log(resistance / THERMISTORNOMINAL);
  steinhart /= BCOEFFICIENT;
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15);
  steinhart = 1.0 / steinhart;
  steinhart -= 273.15;

  return steinhart;
}

void setup() {
  // put your setup code here, to run once:
  XBee.begin(9600);
  Serial.begin(9600);
  analogReference(EXTERNAL);
}

void loop() {
  float reading, average;

  average = getAverage();
  reading = SERIESRESISTOR/average; // Thermistor resistance

  /*
  Serial.print("Thermistor resistance ");
  Serial.println(reading);
  Serial.print("Temperature ");
  */
  reading = steinhart(reading); // Temperature
  sendJson(reading);
  //XBee.write("hi");
  delay(5000);
}
