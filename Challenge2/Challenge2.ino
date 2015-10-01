



#include "EEPROMAnything.h"
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <string.h>
SoftwareSerial XBee(2, 3); // Rx, Tx

#define VERSION 1

#define BAUD_RATE 9600
#define PIN A0
#define VCC 3.3
#define SERIESRESISTOR 9969
#define SAMPLES 10
#define SEARCH_DELAY 100
#define AVERAGE_DELAY 10

#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 25
#define BCOEFFICIENT 3950

int id;

typedef struct {
  int version;
  int id;
} identification;

identification identity;

void sendJson(float reading){
  char buffer[100];
  StaticJsonBuffer<100> jsonBuffer;

  memset(buffer, 0, sizeof(buffer));

  JsonObject& root = jsonBuffer.createObject();
  root["id"] = id;
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

  EEPROM_readAnything(0, identity);

  if(identity.version == VERSION){
    id = identity.id;
    Serial.print("found old id\n");
  } else {
    randomSeed(analogRead(PIN)); // seed value from the thermistor
    id = random(10000,32767); // generate a specific ID for this device

    identity.id = id;
    identity.version = VERSION;

    EEPROM_writeAnything(0, identity);
    Serial.print("write id\n");
  }
}

void loop() {
  float reading, average;

  average = getAverage();
  reading = SERIESRESISTOR/average; // Thermistor resistance

  reading = steinhart(reading); // Temperature
  sendJson(reading);
  XBee.flush();
  delay(2000);
}
