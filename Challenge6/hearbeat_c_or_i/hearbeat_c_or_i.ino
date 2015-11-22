#include <XBee.h>
#include <SoftwareSerial.h>
#include <TimerOne.h>

#define HELLO 0x11
#define LEADER 0x12
#define NODE 0x13
#define INFECT 0x04
#define CLEAR 0x05
#define ELECTION 0x06
#define LEADER_HEARTBEAT_c 0x07
#define LEADER_HEARTBEAT_i 0x08

#define RedPin 7
#define BluePin 8
#define GreenPin 9
#define ButtonPin 10

#define HEARTBEAT_TIME 2000000
#define INFECTION_TIME 3000000

#define ID 8

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();

// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();

//Sending Status of the LED
XBeeAddress64 broadcast64 = XBeeAddress64(0x00000000, 0x0000ffff);

SoftwareSerial XBee(2, 3); // Rx, Tx

int leaderId;
int isLeader = 5;
int timeSinceButton = 0;
bool isInfected = false;
bool spreadInfection = false;
bool bidForLeader = false;
bool waitForNewLeader = false;
unsigned long timeSinceHeartBeat = 0;
unsigned long timeSinceBid = 0;
unsigned long timeSinceCure = 0;



void sendHeartBeat(){
  uint8_t* heartbeat = (uint8_t*) malloc(2);
  if (!isInfected) {
    heartbeat[0] = LEADER_HEARTBEAT_c;
    heartbeat[1] = ID;

    ZBTxRequest heartbeatMsg = ZBTxRequest(broadcast64, heartbeat, 2);
    xbee.send(heartbeatMsg);
  
    Serial.println("Sent clean heartbeat");
  } else {
    heartbeat[0] = LEADER_HEARTBEAT_i;
    heartbeat[1] = ID;

    ZBTxRequest heartbeatMsg = ZBTxRequest(broadcast64, heartbeat, 2);
    xbee.send(heartbeatMsg);
  
    Serial.println("Sent infected heartbeat");
  }
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
  isInfected = false;
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
            isInfected = true;
            // do nothing
          } 
          break;
        }
        case LEADER_HEARTBEAT_c:
        {
          if(!spreadInfection)
          {
            if(isInfected)
            {
              greenON();
            }
            isInfected = false;
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
            }
          }
          
          Serial.println("Clean Leader is still alive");
          break;
        }
        case LEADER_HEARTBEAT_i:
        {
          if(!isLeader && spreadInfection)
          {
            Timer1.stop();
            Timer1.detachInterrupt();
            Serial.println("I stopped spreading the infection");    
            redON(); 
          } else if (!isLeader && !spreadInfection && isInfected){
            redON();
          }
          
          isInfected = true;
          spreadInfection = false;
          timeSinceHeartBeat = millis();
          if((rx.getData(1) > leaderId)){
            waitForNewLeader = false;
            isLeader = 0;
            bidForLeader = false;
            leaderId = rx.getData(1);
            Timer1.stop();
            Timer1.detachInterrupt();
            Serial.println("I'm just a lowly follower");
            redON();
          }
          Serial.println("Infected Leader is still alive");
          break;
        }
      }
    }
  }

  unsigned long timeDiff = millis() - timeSinceHeartBeat;
  //Serial.print("Time diff: ");
  //Serial.println(millis() - timeSinceHeartBeat);
  
  //if(!waitForNewLeader && ((timeDiff) > (HEARTBEAT_TIME/1000) + 750)){
  if(!isLeader && ((timeDiff) > 7000))
  {

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
    //isInfected = false;
    bidForLeader = false;
    Serial.println("I won the election");
    
    blueON();
  }

  if (digitalRead(ButtonPin) == HIGH && (millis() - timeSinceButton) > 250)
  {
    timeSinceButton = millis();
    if(isLeader)
    {
      isInfected = false;
      spreadInfection = false;
      blueON();
    } else if(isInfected)
    {
      // you're already infected, don't do anything
    } else 
    {
      spreadInfection = true;
      isInfected = true;
  
      uint8_t* msg2 = (uint8_t*) malloc(2);
      msg2[0] = INFECT;
      msg2[1] = ID;
  
      ZBTxRequest infectMsg = ZBTxRequest(broadcast64, msg2, 2);
      xbee.send(infectMsg);
  
      Timer1.setPeriod(INFECTION_TIME);
      Timer1.attachInterrupt(sendInfection);
      Serial.println("Just got infected");
      
      redON();
    }

  } 

}
