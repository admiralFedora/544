#include <XBee.h>

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

int Led = 0;
int ON = 255;
int OFF = 0;

void LedON(int pin) 
{
      analogWrite(pin, ON);
}

void LedOFF(int pin) 
{
      analogWrite(pin, OFF);
}

void setup() {
  pinMode(Led, OUTPUT);
  
  // start serial
  Serial.begin(9600);
  xbee.begin(Serial);
  
  LedON(Led);
}

// continuously reads packets, looking for ZB Receive or Modem Status
void loop() {
    
    xbee.readPacket();
    
    if (xbee.getResponse().isAvailable()) {
      // got something
      
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        // got a zb rx packet
        
        // now fill our zb rx class
        xbee.getResponse().getZBRxResponse(rx);
            
        // set dataLed PWM to value of the first byte in the data
        analogWrite(Led, rx.getData(0));

      } else if (xbee.getResponse().getApiId() == MODEM_STATUS_RESPONSE) {
        xbee.getResponse().getModemStatusResponse(msr);
        // the local XBee sends this response on certain events, like association/dissociation
    
      }
    } else if (xbee.getResponse().isError()) {
      //nss.print("Error reading packet.  Error code: ");  
      //nss.println(xbee.getResponse().getErrorCode());
    }
}
