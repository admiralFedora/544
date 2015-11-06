#include <Printers.h>
#include <XBee.h>

#include <XBee.h>
#include <SoftwareSerial.h>

#define BEACONS 3

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();

uint8_t dbCmd[] = {'D','B'};
uint8_t payload[] = { 0 }; //Payload Packet\
uint8_t toSend*;

// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

//Sending Status of the LED
XBeeAddress64 broadcast64 = XBeeAddress64(0x00000000, 0x0000ffff);
ZBTxRequest requestRSSI = ZBTxRequest(broadcast64, payload, sizeof(payload));

XBeeAddress64 addr64;
ZBTxRequest sendRRSIs;

SoftwareSerial XBee(2, 3); // Rx, Tx

bool keepReading = true;

void getAndSendRSSI()
{
	int i = 1;
	toSend[0] = 2 & 0xff;
	while(keepReading)
	{
		if(xbee.readPacket(4000))
		{
			if (xbee.getResponse().isAvailable()) //Packet received
			{
			  Serial.println("Packet Received!");
			  Serial.print("packet type:");
			  Serial.println(xbee.getResponse().getApiId(), HEX);
			  if(xbee.getResponse().getApiId() == ZB_RX_RESPONSE)
			  {
				xbee.getResponse().getZBRxResponse(rx);

				if(rx.getData(0) == 9)
				{
				  Serial.println("I got the data!");
				  Serial.print("Rssi: ");
				  Serial.println(rx.getData(1), HEX);
				  toSend[i] = rx.getData(1) & 0xff;
				  toSend[i+1] = rx.getData(2) & 0xff;
				  i += 2;
				}
			  }
			}
		} else 
		{
			keepReading = false;
		}
	}
	
	sendRRSIs = ZBTxRequest(addr64, toSend, sizeof(toSend));
	xbee.send(sendRSSIs);
}

void setup() 
{ 
  // start serial
  Serial.begin(9600);
  XBee.begin(9600);
  xbee.begin(XBee);

  payload[0] = 8 & 0xff;
  
  toSend = (uint8_t*) malloc(BEACONS * 2 + 1);
}

// looks for an xbee that just joined the network and starts sending to the last one that joined
void loop() 
{
	xbee.readPacket();
	if(xbee.getResponse().isAvailable())
	{
		if(xbee.getResponse().getApiId() == ZB_RX_RESPONSE)
		{
			xbee.getResponse().getZBRxResponse(rx);
			addr64 = rx.getRemoteAddress64();
			
			if(rx.getData(0) == 1)
			{
				xbee.send(requestRSSI);
				getAndSendRSSI();
			}
		}
	}
    
}
