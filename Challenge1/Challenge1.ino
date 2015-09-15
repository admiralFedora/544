#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <string.h>
SoftwareSerial XBee(2, 3); // Rx, Tx

#define PIN A0
#define VCC 3.3
#define SERIESRESISTOR 10000
#define SAMPLES 10
#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 25
#define BCOEFFICIENT 3950
#define IDREQUEST "IDREQUEST"
#define REQUEST "REQUEST"

long id;
bool identified;

void sendJson(float reading){
  char buffer[100];
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["id"] = id;
  root["temp"] = reading;

  root.printTo(buffer, sizeof(buffer));
  XBee.print(buffer);
  Serial.println(buffer);
}

void identify(){
  char buff[9];
  int i;
  while(true){
    // look for the request coming from the server for the ID
    for(i = 0; i < 9 && XBee.available(); i++){
      buff[i] = XBee.read();
    }
    // send the ID once the request has been found
    if(!strcmp(IDREQUEST, buff)){
      XBee.print(id);
      break;
    }
  }
}

bool request(){
  char buff[7];
  int i;
  while(true){
    for(i = 0; i < 7 && XBee.available(); i++){
      buff[i] = XBee.read();
    }
    if(!strcmp(REQUEST, buff)){
      return true;
    }
  }
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
  XBee.begin(9600);
  Serial.begin(9600);
  analogReference(EXTERNAL);
  randomSeed(analogRead(PIN));
  id = random(65535); // generate a specific ID for this device
  identified = false;
}

void loop() {
  float reading, average;

  if(!identified){
    identify();
    identified = true;
  }

  if(request()){
    average = getAverage();
    reading = SERIESRESISTOR/average; // Thermistor resistance

    reading = steinhart(reading); // Temperature
    sendJson(reading);
  }
  //XBee.write("hi");
  delay(5000);
}
