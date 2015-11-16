#include <XBee.h>
#include <SoftwareSerial.h>
#include <TimerOne.h>
#include "LinkedList.h"

#define BEACONS 3

#define HELLO 0x01
#define LEADER 0x02
#define NODE 0x03
#define INFECT 0x04
#define CLEAR 0x05
#define ELECTION 0x06
#define LEADER_HEARTBEAT 0x07

#define ID 1

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();

// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();

//Sending Status of the LED
XBeeAddress64 broadcast64 = XBeeAddress64(0x00000000, 0x0000ffff);

SoftwareSerial XBee(2, 3); // Rx, Tx

Node* head;
int isLeader = 5;
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

void startElection(){
  Node* highest = getHighestNode(head);
  int curId = highest->id;

  uint8_t* msg = (uint8_t*) malloc(2);
  msg[0] = ELECTION;
  msg[1] = ID;
  ZBTxRequest electionRequest;
  
  while(curId != ID){
    electionRequest = ZBTxRequest(highest->addr64, msg, 2);
    xbee.send(electionRequest);

    highest = getNextHighestNode(head, curId);
    curId = highest->id;
  }
}

void setup() 
{ 
  // start serial
  Serial.begin(9600);
  XBee.begin(9600);
  xbee.begin(XBee);

  head = NULL;

  uint8_t* msg = (uint8_t*) malloc(2);
  msg[0] = HELLO;
  msg[1] = ID;

  ZBTxRequest initHello = ZBTxRequest(broadcast64, msg, 2);
  xbee.send(initHello);

  while(xbee.readPacket(5000)){
    if(xbee.getResponse().getApiId() == ZB_RX_RESPONSE){
      xbee.getResponse().getZBRxResponse(rx);

      Node* newNode = (Node*) malloc(sizeof(Node));
      newNode->addr64 = rx.getRemoteAddress64();
      newNode->id = rx.getData(1);
      switch(rx.getData(0)){
        case NODE:
          newNode->leader = false;
          break;
        case LEADER:
          newNode->leader = true;
          isLeader = 0;
          break;
      }
      
      insertNewNode(newNode, &head);
    }
  }

  // didn't receive any info about someone being the leader, therefore you are the leader
  if(isLeader == 5){
    isLeader = 1;
    Timer1.initialize(1500000);
    Timer1.attachInterrupt(sendHeartBeat);
    waitForNewLeader = true;
    Serial.println("I am the one who leads.");
  }
  timeSinceHeartBeat = millis();
  Serial.println("Done with Setup!");
}

// looks for an xbee that just joined the network and starts sending to the last one that joined
void loop() 
{
  xbee.readPacket();
  if(xbee.getResponse().getApiId() == ZB_RX_RESPONSE){
    xbee.getResponse().getZBRxResponse(rx);

    switch(rx.getData(0)){
      case HELLO:
      {
        XBeeAddress64 addr64 = rx.getRemoteAddress64();
        Node* newNode = (Node*) malloc(sizeof(Node));
        newNode->id = rx.getData(1);
        newNode->addr64 = addr64;
        newNode->leader = false;
        insertNewNode(newNode, &head);

        uint8_t* msg = (uint8_t*) malloc(2);
        if(isLeader){
          msg[0] = LEADER;
        } else {
          msg[1] = NODE;
        }
        msg[2] = ID;

        ZBTxRequest initReply = ZBTxRequest(addr64, msg, 2);
        xbee.send(initReply);
        Serial.print("Node ");
        Serial.print(newNode->id);
        Serial.println(" just joined");
        break;
      }
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

          Timer1.initialize(2000000);
          Timer1.attachInterrupt(sendInfection);
          Serial.println("Just got infected");
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
        }
        break;
      }
      case LEADER_HEARTBEAT:
      {
        timeSinceHeartBeat = millis();
        Serial.println("Leader is still alive");
        break;
      }
      case ELECTION:
      {
        if(rx.getData(1) < ID){
          uint8_t* msg = (uint8_t*) malloc(2);
          msg[0] = ELECTION;
          msg[1] = ID;
          ZBTxRequest cancelBidMsg = ZBTxRequest(rx.getRemoteAddress64(), msg, 2);
          xbee.send(cancelBidMsg);
          Serial.println("Cancelling someone's bid");
        } else {
          bidForLeader = false;
          waitForNewLeader = true;
          Serial.println("My Bid was Canceled");
        }
        break;
      }
      case LEADER:
      {
        Node* leader = findNodeById(rx.getData(1), head);
        leader->leader = true;
        timeSinceHeartBeat = millis();
        waitForNewLeader = false;
        Serial.println("A new leader has arrived");
        break;
      }
    }
  }

  if(!waitForNewLeader && ((millis() - timeSinceHeartBeat) > 2500)){
    bidForLeader = true;
    Node* deadLeader = findLeader(head);
    if(deadLeader){
      removeNode(deadLeader, &head);
    }
    timeSinceBid = millis();
    startElection();
  }

  if(bidForLeader && ((millis() - timeSinceBid) > 2500)){
    isLeader = 1;
    isInfected = false;
    waitForNewLeader = true;
    Timer1.detachInterrupt();
    Timer1.attachInterrupt(sendHeartBeat);

    uint8_t* msg = (uint8_t*) malloc(2);
    msg[0] = LEADER;
    msg[1] = ID;

    ZBTxRequest leaderMsg = ZBTxRequest(broadcast64, msg, 2);
    xbee.send(leaderMsg);
  }
}
