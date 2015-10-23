#include <Servo.h>
#include <Wire.h>

#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.

#define LIDAR_FRONT 2
#define LIDAR_BACK 3


Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
bool startup = true; // used to ensure startup only happens once
int startupDelay = 1000; // time to pause at each calibration step
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'
int frontDist = 0; 
int backDist = 0;

// wheel angle > 90 turns left (facing forward)
// wheel angle < 90 turns right (facing forward)

// speed > 90 is backwards (steering in front)
// speed < 90 is forwards (steering in front)
 
void setup()
{
  Serial.begin(9600);
  pinMode(2, INPUT);
  Serial.println("< START >");
  
  
  wheels.attach(8); // initialize wheel servo to Digital IO Pin #8
  esc.attach(9); // initialize ESC to Digital IO Pin #9
  /*  If you're re-uploading code via USB while leaving the ESC powered on, 
   *  you don't need to re-calibrate each time, and you can comment this part out.
   */
  calibrateESC();

  
  // LIDAR control
  Wire.begin(); // join i2c bus

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  digitalWrite(2, LOW);
  digitalWrite(3, LOW);

  frontDist = getLidarDistance(LIDAR_FRONT);
  backDist = getLidarDistance(LIDAR_BACK);
}

/* Calibrate the ESC by sending a high signal, then a low, then middle.*/
void calibrateESC()
{
    esc.write(180); // full backwards
    delay(startupDelay);
    esc.write(0); // full forwards
    delay(startupDelay);
    esc.write(90); // neutral
    delay(startupDelay);
    esc.write(90); // reset the ESC to neutral (non-moving) value
    wheels.write(82); // offset the trim of the wheels
}

// Get a measurement from the LIDAR Lite
int lidarGetRange(void)
{
  int val = -1;
  
  Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
  Wire.write((int)RegisterMeasure); // sets register pointer to  (0x00)  
  Wire.write((int)MeasureValue); // sets register pointer to  (0x00)  
  Wire.endTransmission(); // stop transmitting

  delay(20); // Wait 20ms for transmit

  Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
  Wire.write((int)RegisterHighLowB); // sets register pointer to (0x8f)
  Wire.endTransmission(); // stop transmitting

  delay(20); // Wait 20ms for transmit
  
  Wire.requestFrom((int)LIDARLite_ADDRESS, 2); // request 2 bytes from LIDAR-Lite

  if(2 <= Wire.available()) // if two bytes were received
  {
    val = Wire.read(); // receive high byte (overwrites previous reading)
    val = val << 8; // shift high byte to be high 8 bits
    val |= Wire.read(); // receive low byte as lower 8 bits
  }
  
  return val;
}


int getLidarDistance(int sensor){
  digitalWrite(LIDAR_FRONT, LOW);
  digitalWrite(LIDAR_BACK, LOW);

  digitalWrite(sensor, HIGH);
  delay(10); // delay for turning on sensor

  return lidarGetRange();
}


void driveStraight()
{
  esc.write(90 + 10);
  
  if (frontDist > backDist + 6)
  {
    wheels.write(90 + 50);
    delay(50);
  }
  else if (frontDist < backDist - 6)
  {
    wheels.write(90 - 50);
    delay(50);
  }
  else
  {
    wheels.write(90);
    delay(50);
  }
  frontDist = getLidarDistance(LIDAR_FRONT);
  backDist =  getLidarDistance(LIDAR_BACK);

  
  Serial.print("\nFront:");
  Serial.print(frontDist);
  Serial.print("\nBack:");
  Serial.print(backDist);
}
 
void loop()
{
   // oscillate();
   if (startup)
   {
    driveStraight();
   }
   
   /* (getLidarDistance(LIDAR_FRONT)<10 || getLidarDistance(LIDAR_BACK)<10) 
   {
    startup = false;
     esc.write(90);
   }*/

   delay(100);
}


