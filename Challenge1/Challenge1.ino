#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <string.h>
SoftwareSerial XBee(2, 3); // Rx, Tx

#define BAUD_RATE 9600
#define PIN A0
#define VCC 3.3
#define SERIESRESISTOR 10000
#define SAMPLES 10
#define SEARCH_DELAY 500
#define AVERAGE_DELAY 10

#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 25
#define BCOEFFICIENT 3950

#define IDREQUEST "IDREQUEST"
#define REQUEST "REQUEST"

long id;
bool identified;
char phrase[13];

void sendJson(float reading){
  char buffer[100];
  StaticJsonBuffer<200> jsonBuffer;

  memset(buffer, 0, sizeof(buffer));

  JsonObject& root = jsonBuffer.createObject();
  root["id"] = id;
  root["temp"] = reading;

  root.printTo(buffer, sizeof(buffer));
  XBee.println(buffer);
  Serial.println(buffer);
}

void identify(){
  StaticJsonBuffer<200> jsonBuffer;
  char json[100];
  char buff[10];
  int i;
  bool run;
  run = true;

  memset(json, 0, sizeof(json));
  memset(buff, 0, sizeof(buff));

  while(run){
    // look for the request coming from the server for the ID
    for(i = 0; i < 10 && XBee.available(); i++){
      buff[i] = XBee.read();
    }
    // send the ID once the request has been found
    if(strcmp(IDREQUEST, buff) == 0){
      XBee.print(id);
      run = false;
    }
    memset(buff, 0, sizeof(buff));
    XBee.flush();
    delay(SEARCH_DELAY);
  }

  JsonObject& root = jsonBuffer.createObject();
  root["id"] = id;

  root.printTo(json, sizeof(json));
  XBee.println(json);
}

bool request(){
  char buff[14];
  int i;
  while(true){
    for(i = 0; i < 14 && XBee.available(); i++){
      buff[i] = XBee.read();
    }
    if(strcmp(REQUEST, phrase) == 0){
      return true;
    }
    delay(SEARCH_DELAY);
  }
}

float getAverage(){
  int i;
  float average;
  average = 0;
  for(i = 0; i < SAMPLES; i++){
    average += analogRead(PIN);
    delay(AVERAGE_DELAY);
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
  XBee.begin(BAUD_RATE);
  Serial.begin(BAUD_RATE);
  analogReference(EXTERNAL);
  randomSeed(analogRead(PIN)); // seed value from the thermistor
  id = random(0,65535); // generate a specific ID for this device
  memset(phrase, 0, sizeof(phrase));
  // create the phrase we'll be looking for
  sprintf(phrase, "%s%05Ls", REQUEST, id);
  identified = false;
}

void loop() {
  float reading, average;

  Serial.print("identifiying...");
  if(!identified){
    identify();
    identified = true;
  }
  Serial.print("identified");
  if(request()){
    average = getAverage();
    reading = SERIESRESISTOR/average; // Thermistor resistance

    reading = steinhart(reading); // Temperature
    sendJson(reading);
  }
}
