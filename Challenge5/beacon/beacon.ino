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
ZBTxRequest zbTx;

SoftwareSerial XBee(2, 3); // Rx, Tx

bool startSending = false;

void setup() 
{ 
  // start serial
  Serial.begin(9600);
  XBee.begin(9600);
  xbee.begin(XBee);
}

// looks for an xbee that just joined the network and starts sending to the last one that joined
void loop() 
{
    
    xbee.readPacket();
    
    if (xbee.getResponse().isAvailable()) //Packet received
    {
      if(xbee.getResponse().getApiId() == ZB_IO_NODE_IDENTIFIER_RESPONSE)
      {
        addr64 = rx.getRemoteAddress64();
        zbTx = ZBTxRequest(addr64, payload, sizeof(payload));

        startSending = true;        
      }
    }

    if(startSending){
      payload[0] = 1 & 0xff;
      xbee.send(zbTx);

      // send a message every 5 seconds
      delay(5);
    }
}
