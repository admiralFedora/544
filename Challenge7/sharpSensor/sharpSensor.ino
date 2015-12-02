#include <XBee.h>
#include <SoftwareSerial.h>

#define TURN 0x07

#define ID 1

XBee xbee = XBee();
// XBeeResponse response = XBeeResponse();

// create reusable response objects for responses we expect to handle 
// ZBRxResponse rx = ZBRxResponse();

//Sending Status of the LED
XBeeAddress64 broadcast64 = XBeeAddress64(0x00000000, 0x0000ffff);

SoftwareSerial XBee(2, 3); // Rx, Tx

int ir = 5;
int led = 11;
int data = 0;
int distance = 0;

void sendTurnSignal()
{
  uint8_t* turnSignal = (uint8_t*) malloc(2);
  turnSignal[0] = TURN;
  turnSignal[1] = ID;
  
  ZBTxRequest turnSignalMsg = ZBTxRequest(broadcast64, turnSignal, 2);
  xbee.send(turnSignalMsg);
  
  Serial.println("Sent turn Signal");
}

void setup() 
{
  Serial.begin(9600);
  XBee.begin(9600);
  xbee.begin(XBee);
  
  pinMode(led, OUTPUT);
  pinMode(ir,INPUT);

  Serial.println("Done with Setup!");

}

void loop() 
{
  data = analogRead(ir);
  distance = map(data,512,0,15,150);    
  if(distance<115)
  {
    digitalWrite(led, HIGH);
    sendTurnSignal();
    delay(1000);
  }
  else
  {
      digitalWrite(led, LOW);
  }
  Serial.println(distance);
  delay(200);
}

