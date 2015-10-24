#include <Servo.h>
#include <Wire.h>

#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.


Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
bool startup = true; // used to ensure startup only happens once
int startupDelay = 1000; // time to pause at each calibration step
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'
int frontDist = 0; 
int backDist = 0;

int centerpoint = 90; //CALIBRATED CENTER
int Drive = 50; //OUTPUT

// wheel angle > 90 turns left (facing forward)
// wheel angle < 90 turns right (facing forward)

// speed > 90 is backwards (steering in front)
// speed < 90 is forwards (steering in front)
 
void setup()
{
  Serial.begin(9600);
  pinMode(2, INPUT);
  
  wheels.attach(8); // initialize wheel servo to Digital IO Pin #8
  esc.attach(9); // initialize ESC to Digital IO Pin #9
  /*  If you're re-uploading code via USB while leaving the ESC powered on, 
   *  you don't need to re-calibrate each time, and you can comment this part out.
   */
  calibrateESC();

  pinMode(5, OUTPUT); // set up lidar sensor 1 (front)
  pinMode(6, OUTPUT); // set up lidar sensor 2 (back)

  digitalWrite(5, LOW);
  digitalWrite(6, LOW);  

  // LIDAR control
  Wire.begin(); // join i2c bus

  frontDist = lidarGetRange(1);
  backDist = lidarGetRange(2);
}

// Get a measurement from the LIDAR Lite
int lidarGetRange(int sensor)
{
  digitalWrite(6, LOW);
  digitalWrite(5, LOW);
  switch(sensor)
  {
    case 1:
      digitalWrite(5, HIGH);
    case 2:
      digitalWrite(6, HIGH);
  }
  delay(1);
  
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

/* Convert degree value to radians */
double degToRad(double degrees){
  return (degrees * 71) / 4068;
}

/* Convert radian value to degrees */
double radToDeg(double radians){
  return (radians * 4068) / 71;
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

/* Oscillate between various servo/ESC states, using a sine wave to gradually 
 *  change speed and turn values.
 */
void oscillate()
{
  for (int i =0; i < 360; i++){
    double rad = degToRad(i);
    double speedOffset = sin(rad) * maxSpeedOffset;
    double wheelOffset = sin(rad) * maxWheelOffset;
    esc.write(90 + speedOffset);
    wheels.write(90 + wheelOffset);
    delay(50);
  }
}

void driveStraight()
{
  esc.write(90 + 10);
  
  if (frontDist > backDist + 3)
  {
    wheels.write(centerpoint + Drive);
    delay(50);
  }
  else if (frontDist < backDist - 3)
  {
    wheels.write(centerpoint - Drive);
    delay(50);
  }
  else
  {
    wheels.write(90);
    delay(50);
  }
  frontDist = lidarGetRange(1);
  backDist = lidarGetRange(2);
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
   else if (lidarGetRange(1)<3 || lidarGetRange(2)<3) 
   {
    startup = false;
    esc.write(90);
   }

   delay(500);
}


