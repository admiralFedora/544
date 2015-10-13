#include <XBee.h>

XBee xbee = XBee(); // Create an XBee object at the top of your sketch
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
Rx16Response rx = Rx16Response();
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

//void LedStatus(int pin)
//{
//  analogRead(pin);
//}

/*
 * void sendJson(float reading){
  char buffer[100];
  StaticJsonBuffer<100> jsonBuffer;

  memset(buffer, 0, sizeof(buffer));

  JsonObject& root = jsonBuffer.createObject();
  root["id"] = id;
  root["temp"] = reading;

  root.printTo(buffer, sizeof(buffer));
  XBee.println(buffer);
  Serial.println(buffer);
}
 */

void setup() {
  pinMode(Led, OUTPUT);
  
  // start serial port
  Serial.begin(9600);
  xbee.begin(Serial);
  
  LedON(Led);
}

// continuously reads packets, looking for ZB Receive or Modem Status
void loop() {
    
    xbee.readPacket();
    
    xbee.getResponse().getRx16Response(rx);
    //Serial.print(rx);
    //Serial.println("in the loop");
    Serial.println(xbee.getResponse().getApiId());

    if (xbee.getResponse().isAvailable()) //Packet received 
    {
      
      //Packet is of an LED command type
      if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
        // got a zb rx packet
        
        // now fill our zb rx class
        xbee.getResponse().getRx16Response(rx);
/*
        if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) 
        {
          // the sender got an ACK
       
        } 
        else 
        {
            // we got it (obviously) but sender didn't get an ACK
        
        }
            
        // set dataLed PWM to value of the first byte in the data
        analogWrite(Led, rx.getData(0));
*/

      } 
/*
      //Packet is of a status request type
      else if (xbee.getResponse().getApiId() == MODEM_STATUS_RESPONSE) 
      {
        xbee.getResponse().getModemStatusResponse(msr);
        // the local XBee sends this response on certain events, like association/dissociation
    
      }
*/
    } else if (xbee.getResponse().isError()) {
      //nss.print("Error reading packet.  Error code: ");  
      //nss.println(xbee.getResponse().getErrorCode());
    }
}
