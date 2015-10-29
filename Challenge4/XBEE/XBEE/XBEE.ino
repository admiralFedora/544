#include <XBee.h>
#include <SoftwareSerial.h>
#define OPIN 8

XBee xbee = XBee();
SoftwareSerial XBee(2, 3); // Rx, Tx

ZBRxResponse rx = ZBRxResponse();

void setup() {
  // put your setup code here, to run once:
  XBee.begin(9600);
  xbee.begin(XBee);

  pinMode(OPIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
xbee.readPacket();
   if(xbee.getResponse().isAvailable())
   {
      if(xbee.getResponse().getApiId() == ZB_RX_RESPONSE)
      {
          xbee.getResponse().getZBRxResponse(rx);

          if(rx.getOption() == ZB_PACKET_ACKNOWLEDGED)
          {
             switch(rx.getData(0))
             {
              case 0:
                digitalWrite(OPIN,LOW); 
                break;
              case 1:
                digitalWrite(OPIN, HIGH);
                break;
             }
          }
      }
   }
}
