#include <XBee.h>
#include <SoftwareSerial.h>
#include <TimerOne.h>

#define HELLO 0x11
#define LEADER 0x12
#define NODE 0x13
#define INFECT 0x04
#define CLEAR 0x05
#define ELECTION 0x06
#define LEADER_HEARTBEAT 0x07

#define RedPin 7
#define BluePin 8
#define GreenPin 9
#define ButtonPin 10

#define HEARTBEAT_TIME 3000000
#define INFECTION_TIME 2000000

#define ID 3

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();

// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();

//Sending Status of the LED
XBeeAddress64 broadcast64 = XBeeAddress64(0x00000000, 0x0000ffff);

SoftwareSerial XBee(2, 3); // Rx, Tx

int leaderId;
int isLeader = 5;
int buttonState = 0;
bool isInfected = false;
bool bidForLeader = false;
bool waitForNewLeader = false;
unsigned long timeSinceHeartBeat = 0;
unsigned long timeSinceBid = 0;

void sendHeartBeat(){
  uint8_t* heartbeat = (uint8_t*) malloc(2);
  heartbeat[0] = LEADER_HEARTBEAT;
  heartbeat[1] = ID;

  ZBTxRequest heartbeatMsg = ZBTxRequest(broadcast64, heartbeat, 2);
  xbee.send(heartbeatMsg);
  Serial.println("Sent heartbeat");
}

void sendInfection(){
  uint8_t* infection = (uint8_t*) malloc(2);
  infection[0] = INFECT;
  infection[1] = ID;

  ZBTxRequest infectMsg = ZBTxRequest(broadcast64, infection, 2);
  xbee.send(infectMsg);
  Serial.println("Sent my infection");
}

void redON()
{
  digitalWrite(RedPin, HIGH);
  digitalWrite(BluePin, LOW);
  digitalWrite(GreenPin, LOW);
}
void blueON()
{
  digitalWrite(BluePin, HIGH);
  digitalWrite(RedPin, LOW);
  digitalWrite(GreenPin, LOW);
}
void greenON()
{
  digitalWrite(GreenPin, HIGH);
  digitalWrite(RedPin, LOW);
  digitalWrite(BluePin, LOW);
}

void setup() 
{ 

  pinMode(RedPin, OUTPUT);
  pinMode(BluePin, OUTPUT);
  pinMode(GreenPin, OUTPUT);
  pinMode(ButtonPin, INPUT);

  
  digitalWrite(RedPin, LOW);
  digitalWrite(BluePin, LOW);
  digitalWrite(GreenPin, LOW);
  
  // start serial
  Serial.begin(9600);
  XBee.begin(9600);
  xbee.begin(XBee);

  // didn't receive any info about someone being the leader, therefore you are the leader
  isLeader = 1;
  leaderId = ID;
  Timer1.initialize(HEARTBEAT_TIME);
  Timer1.attachInterrupt(sendHeartBeat);
  waitForNewLeader = true;
  Serial.println("I am the one who leads.");
  timeSinceHeartBeat = millis();
  timeSinceBid = millis();
  Serial.println("Done with Setup!");
  bidForLeader = true;
  greenON();
}

// looks for an xbee that just joined the network and starts sending to the last one that joined
void loop() 
{
  xbee.readPacket();
  if(xbee.getResponse().isAvailable()){
    if(xbee.getResponse().getApiId() == ZB_RX_RESPONSE){
      xbee.getResponse().getZBRxResponse(rx);
      Serial.print("Command type: ");
      Serial.println(rx.getData(0), HEX);
      switch(rx.getData(0)){
        case INFECT:
        {
          if(isLeader){
            // do nothing
          } else if(isInfected){
            // you're already infected, don't do anything
          } else {
            isInfected = true;
  
            uint8_t* msg2 = (uint8_t*) malloc(2);
            msg2[0] = INFECT;
            msg2[1] = ID;
  
            ZBTxRequest infectMsg = ZBTxRequest(broadcast64, msg2, 2);
            xbee.send(infectMsg);
  
            Timer1.initialize(INFECTION_TIME);
            Timer1.attachInterrupt(sendInfection);
            Serial.println("Just got infected");

            redON();
          }
          break;
        }
        case CLEAR:
        {
          if(isLeader){
            // do nothing
          } else {
            Timer1.detachInterrupt();
            isInfected = false;
  
            uint8_t* msg = (uint8_t*) malloc(2);
            msg[0] = CLEAR;
            msg[1] = ID;
  
            ZBTxRequest clearMsg = ZBTxRequest(broadcast64, msg, 2);
            xbee.send(clearMsg);
  
            Serial.println("Just got cleared");
            
            greenON();
          }
          break;
        }
        case LEADER_HEARTBEAT:
        {
          timeSinceHeartBeat = millis();
          if((rx.getData(1) > leaderId)){
            waitForNewLeader = false;
            isLeader = 0;
            bidForLeader = false;
            leaderId = rx.getData(1);
            Timer1.stop();
            Timer1.detachInterrupt();
            Serial.println("I'm just a lowly follower");
            greenON();
            if(isInfected){
              Timer1.setPeriod(INFECTION_TIME);
              Timer1.attachInterrupt(sendInfection);
            }
          }
          Serial.println("Leader is still alive");
          break;
        }
      }
    }
  }

  if(!waitForNewLeader && ((millis() - timeSinceHeartBeat) > (HEARTBEAT_TIME/1000) + 500)){
    Serial.println("Leader died. I'm electing myself.");
    bidForLeader = true;
    waitForNewLeader = true;
    timeSinceBid = millis();
    isLeader = 1;
    leaderId = ID;

    Timer1.stop();
    Timer1.detachInterrupt();
    Timer1.setPeriod(HEARTBEAT_TIME);
    Timer1.attachInterrupt(sendHeartBeat);
  }

  // you're still in the race and it's been a while since the election started
  if(bidForLeader && ((millis() - timeSinceBid) > 2500)){
    // clear the infection, you're the leader now
    isInfected = false;
    bidForLeader = false;
    Serial.println("I won the election");
    
    blueON();
  }

  //buttonState = digitalRead(ButtonPin);
  if (digitalRead(ButtonPin) == HIGH && buttonState == 0)
  {
    if(isLeader){
      uint8_t* msg2 = (uint8_t*) malloc(2);
      msg2[0] = CLEAR;
      msg2[1] = ID;
  
      ZBTxRequest clearMsg = ZBTxRequest(broadcast64, msg2, 2);
      xbee.send(clearMsg);
  
      Timer1.initialize(2000000);
      Timer1.attachInterrupt(sendInfection);
      Serial.println("Just got cured");
      
      greenON();
      
    } else if(isInfected){
      // you're already infected, don't do anything
    } else {
      isInfected = true;
  
      uint8_t* msg2 = (uint8_t*) malloc(2);
      msg2[0] = INFECT;
      msg2[1] = ID;
  
      ZBTxRequest infectMsg = ZBTxRequest(broadcast64, msg2, 2);
      xbee.send(infectMsg);
  
      Timer1.initialize(INFECTION_TIME);
      Timer1.attachInterrupt(sendInfection);
      Serial.println("Just got infected");
      
      redON();
    }

  } else {
    buttonState = 0;
  }

}
