//SENDING
#include "EEPROMAnything.h"
// # include <Printers.h>
#include <XBee.h>
#include <SoftwareSerial.h>

#define ID 3

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();

uint8_t dbCmd[] = {'D','B'};
uint8_t payload[] = { 5, 0, 0 }; //Payload Packet

// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

AtCommandRequest dbRequest = AtCommandRequest(dbCmd);
AtCommandResponse dbResponse = AtCommandResponse();

//Sending Status of the LED
XBeeAddress64 addr64;
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));

SoftwareSerial XBee(2, 3); // Rx, Tx

bool startSending = false;

uint8_t readDB(){
  xbee.send(dbRequest);

  if(xbee.readPacket(5000)){

    if(xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE){
      xbee.getResponse().getAtCommandResponse(dbResponse);

      if(dbResponse.isOk()){
        if(dbResponse.getValueLength() > 0){
          Serial.print("RSSI: ");
          Serial.println(dbResponse.getValue()[0],HEX);
          return dbResponse.getValue()[0];
        }
      }
    }
  } 
}

void setup() 
{ 
  // start serial
  Serial.begin(9600);
  XBee.begin(9600);
  xbee.begin(XBee);

  payload[0] = 9 & 0xff;
}

// looks for an xbee that just joined the network and starts sending to the last one that joined
void loop() 
{
    
    xbee.readPacket();
    
    if (xbee.getResponse().isAvailable()) //Packet received
    {
      Serial.println("Packet Received");
      if(xbee.getResponse().getApiId() == ZB_RX_RESPONSE){
        Serial.println("It's a response");
        xbee.getResponse().getZBRxResponse(rx);
        addr64 = rx.getRemoteAddress64();
        
        Serial.println(addr64.getMsb(), HEX);
        
        if(rx.getData(0) == 8){
          Serial.println("Sending data");
		      payload[1] = ID & 0xff;
          payload[2] = readDB() & 0xff;
          zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
          xbee.send(zbTx);
        }
      }
    }
}
