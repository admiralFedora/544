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

AtCommandRequest dbRequest = AtCommandRequest(dbCmd);
AtCommandResponse dbResponse = AtCommandResponse();

//Sending Status of the LED
XBeeAddress64 addr64 = XBeeAddress64();
ZBTxRequest zbTx;

SoftwareSerial XBee(2, 3); // Rx, Tx

void readDB(){
  xbee.send(dbRequest);

  if(xbee.readPacket(5000)){

    if(xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE){
      xbee.getResponse().getAtCommandResponse(dbResponse);

      if(dbResponse.isOk()){
        if(dbResponse.getValueLength() > 0){
          Serial.print("RSSI: ");
          Serial.print(dbResponse.getValue()[0],HEX);
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
}

// looks for an xbee that just joined the network and starts sending to the last one that joined
void loop() 
{
    xbee.readPacket();
    
    if (xbee.getResponse().isAvailable()) //Packet received
    {
      Serial.println("Packet Received!");
      if(xbee.getResponse().getApiId() == ZB_RX_RESPONSE)
      {

        xbee.getResponse().getZBRxResponse(rx);
        addr64 = rx.getRemoteAddress64();
        
        if(rx.getOption() == ZB_PACKET_ACKNOWLEDGED)
        {
          //Serial.println();
          
          switch(rx.getData(0))
          {
            case 5:
              readDB();
              break;
          }
        }
      }
    }

}
