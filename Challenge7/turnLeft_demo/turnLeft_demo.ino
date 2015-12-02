#include <XBee.h>
#include <SoftwareSerial.h>

#include <Servo.h>
#include <Wire.h>
#include <math.h>

#define centerpoint 82 //CALIBRATED CENTER
#define motorSpeed -10

Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
volatile bool startup = true; // used to ensure startup only happens once
volatile bool startRun = true;


// wheel angle > 90 turns left (facing forward)
// wheel angle < 90 turns right (facing forward)

// speed > 90 is backwards (steering in front)
// speed < 90 is forwards (steering in front)


#define TURN 0x07

#define ID 3

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();

// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();

//Sending Status of the LED
XBeeAddress64 broadcast64 = XBeeAddress64(0x00000000, 0x0000ffff);

SoftwareSerial XBee(2, 3); // Rx, Tx

int led = 11;

void setup() 
{
  Serial.begin(9600);
  XBee.begin(9600);
  xbee.begin(XBee);
  
  pinMode(led, OUTPUT);

  wheels.attach(8); // initialize wheel servo to Digital IO Pin #8
  esc.attach(9); // initialize ESC to Digital IO Pin #9
  /*  If you're re-uploading code via USB while leaving the ESC powered on, 
   *  you don't need to re-calibrate each time, and you can comment this part out.
   */
  calibrateESC();


  Serial.println("Done with Setup!");

}

void calibrateESC()
{
  int startupDelay = 1000; // time to pause at each calibration step
    esc.write(180); // full backwards
    delay(startupDelay);
    esc.write(0); // full forwards
    delay(startupDelay);
    esc.write(90); // neutral
    delay(startupDelay);
    esc.write(90); // reset the ESC to neutral (non-moving) value
    wheels.write(82); // offset the trim of the wheels
}

void driveStraight()
{
  wheels.write(centerpoint);
  esc.write(centerpoint + motorSpeed);
  
}

void turnLeft()
{
  wheels.write(135);
  esc.write(centerpoint + motorSpeed);
  delay(1000);
}

void loop() 
{
  xbee.readPacket();
  if(xbee.getResponse().isAvailable())
  {
    if(xbee.getResponse().getApiId() == ZB_RX_RESPONSE)
    {
      xbee.getResponse().getZBRxResponse(rx);
      Serial.print("Command type: ");
      Serial.println(rx.getData(0), HEX);
      switch(rx.getData(0))
      {
        case TURN:
          digitalWrite(led, HIGH);
          Serial.println("Turning!!!!!!!!!!!!");
          turnLeft();
          delay(3000);
          break;
      }
    }
  }
  else
  {
    driveStraight();
    Serial.println("No Turn Signal");
    digitalWrite(led, LOW);
    delay(200);
  }

}

