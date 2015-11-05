#include <XBee.h>

//***However, to broadcast to the coordinator we will need to set the address to 
//address: XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x00000000); 

XBee xbee = XBee();

uint8_t payload[] = { 0, 0 };


XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x0000ffff);//TRY THIS
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();


void setup() {
  xbee.begin(9600);
}

void loop()
{   
    
    xbee.send(zbTx);
  
    delay(1000);
}
