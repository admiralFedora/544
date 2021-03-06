#include <Printers.h>
#include <XBee.h>

#include <XBee.h>
#include <SoftwareSerial.h>

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();

uint8_t payload[] = { 0 }; //Payload Packet

// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

//Sending Status of the LED
XBeeAddress64 addr64 = XBeeAddress64();
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));

SoftwareSerial XBee(2, 3); // Rx, Tx

int LED = 7; //GPIO pin 2
int Status = 0; //Initializing Status to OFF

void LEDON(int pin) 
{
    digitalWrite(pin, HIGH);
}

void LEDOFF(int pin) 
{
    digitalWrite(pin, LOW);
}

void setup() 
{
  pinMode(LED, OUTPUT); //setting GPIO pin to Output
  digitalWrite(LED, LOW);
  
  // start serial
  Serial.begin(9600);
  XBee.begin(9600);
  xbee.begin(XBee);

}

// continuously reads packets, looking for ZB Receive or Modem Status
void loop() 
{
    
    xbee.readPacket();
    
    if (xbee.getResponse().isAvailable()) //Packet received
    {
      if(xbee.getResponse().getApiId() == ZB_RX_RESPONSE)
      {

        xbee.getResponse().getZBRxResponse(rx);
        /* DELETE
        char test[64];
        sprintf(test, "help 0%x", rx.getRemoteAddress64());
        //sprintf(test, rx.getRemoteAddress64(), sizeof(rx.getRemoteAddress64()));
        Serial.println(test);*/
        addr64 = rx.getRemoteAddress64();
        
        if(rx.getOption() == ZB_PACKET_ACKNOWLEDGED)
        {
          Serial.println("Packet Received!");
          //Serial.println();
          
          switch(rx.getData(0))
          {
            case 0:
              // if packet contains a 0, turn OFF the LED
              //digitalWrite(LED, LOW);
              LEDOFF(LED);
              Serial.println("LED OFF");
              break;
            case 1:
              // if packet contains a 1, turn ON the LED
              //digitalWrite(LED, HIGH);
              LEDON(LED);
              Serial.println("LED ON");
              break;
            case 8: //REQUEST Status
              // if packet contains an 8, read in the status of the LED
              Status = digitalRead(LED);
              Serial.println("LED Status:");
              Serial.println(Status);
              
              //Sending
              payload[0] = Status & 0xff;
              xbee.send(zbTx);
             
              break;
          }
        } else {
          Serial.println("oh no");
        }
        
      }
    } else {
      //Serial.println("nothing");
    }
}
