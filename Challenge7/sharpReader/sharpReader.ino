#include <XBee.h>
#include <SoftwareSerial.h>

#define TURN 0x07

#define ID 8

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();

// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();

//Sending Status of the LED
XBeeAddress64 broadcast64 = XBeeAddress64(0x00000000, 0x0000ffff);

SoftwareSerial XBee(2, 3); // Rx, Tx

int ledPIN = 11;
int piPIN = 7;


void setup() 
{
  Serial.begin(9600);
  XBee.begin(9600);
  xbee.begin(XBee);
  
  pinMode(ledPIN, OUTPUT);
  pinMode(piPIN, OUTPUT);

  digitalWrite(piPIN, LOW);

  Serial.println("Done with Setup!");

}

void loop() 
{
  xbee.readPacket();
  if(xbee.getResponse().isAvailable())
  {
    if(xbee.getResponse().getApiId() == ZB_RX_RESPONSE)
    {
      xbee.getResponse().getZBRxResponse(rx);
      Serial.print("Command type: ");
      Serial.println(rx.getData(0), HEX);
      switch(rx.getData(0))
      {
        case TURN:
          digitalWrite(ledPIN, HIGH);
          digitalWrite(piPIN, HIGH);
          Serial.println("Turning!!!!!!!!!!!!");
          delay(1500);
          break;
      }
    }
  }
  else
  {
    digitalWrite(ledPIN, LOW);
    digitalWrite(piPIN, LOW);
    Serial.println("No Turn Signal");
    delay(200);
  }
}

