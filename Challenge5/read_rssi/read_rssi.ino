#include <Printers.h>
#include <XBee.h>

#include <XBee.h>
#include <SoftwareSerial.h>

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();

uint8_t dbCmd[] = {'D','B'};
uint8_t payload[] = { 0 }; //Payload Packet

// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

//Sending Status of the LED
XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x0000ffff);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));

SoftwareSerial XBee(2, 3); // Rx, Tx

bool keepReading = true;

void setup() 
{ 
  // start serial
  Serial.begin(9600);
  XBee.begin(9600);
  xbee.begin(XBee);

  payload[0] = 8 & 0xff;
}

// looks for an xbee that just joined the network and starts sending to the last one that joined
void loop() 
{
    xbee.send(zbTx);

    for(int i = 0; i < 2; i++){
      if(xbee.readPacket(5000)){
        if (xbee.getResponse().isAvailable()) //Packet received
        {
          Serial.println("Packet Received!");
          Serial.print("packet type:");
          Serial.println(xbee.getResponse().getApiId(), HEX);
          if(xbee.getResponse().getApiId() == ZB_RX_RESPONSE)
          {
            xbee.getResponse().getZBRxResponse(rx);
  
            if(rx.getData(0) == 9){
              Serial.println("I got the data!");
              Serial.print("Rssi: ");
              Serial.println(rx.getData(1), HEX);
            }
          }
        }
      }
    }
    
}
