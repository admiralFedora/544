#include <Servo.h>
#include <Wire.h>
#include <math.h>
#include "Fifo.h"

#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.
#define    pi 3.1459
//Pins
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
int deltaFrontBack = 0;
int motorSpeed = -15;

//PID Initializations
int centerpoint = 82; //CALIBRATED CENTER
int Output = 82; //OUTPUT
int pOutput = 82; // PREVIOUS OUTPUT

int thetaDesired = 0;//Zero degree being straight down the hallway, left = -45, right 45
float thetaActual;
float distanceDesired;
float distanceActual;
float thetaTurn;
float thetaMax = pi/16;

float Error = 0.0;
float pError = 0.0;
float distanceError;
float thetaError;
float maxError;
float minError;
float K_p = 2;
float K_i = 0;
float K_d = 0.5;
float Integral;
float Derivative;
float dt = 0.160;

float lengthbetweensensors = 20.0;//in centimeters, must be the same unit as getLidarDistance 

Fifo *front, *back;

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

  initReadings(LIDAR_FRONT, &front);
  initReadings(LIDAR_BACK, &back);

  deltaFrontBack_calc();
    

  //distanceDesired = getLidarDistance(LIDAR_FRONT);
  distanceDesired = 50.0;
  
  maxError = 1.0 * distanceDesired;
  minError = -1.0 * distanceDesired;
}

void initReadings(int sensor, Fifo **fifo){
  int i;
  Fifo *temp;
  *fifo = (Fifo*) malloc(sizeof(Fifo));
  (*fifo)->value = 0;
  (*fifo)->next = NULL;
  for(i = 0; i < 4; i++){
    temp = (Fifo*) malloc(sizeof(Fifo));
    temp->value = i + 1;
    temp->next = *fifo;
    *fifo = temp;
  }
}

void deltaFrontBack_calc()
{
  // get one new reading and remove the oldest
  Fifo *newFront = (Fifo*) malloc(sizeof(Fifo));
  newFront->value = getLidarDistance(LIDAR_FRONT);
  insertAndPop(newFront, &front);

  Fifo *newBack = (Fifo*) malloc(sizeof(Fifo));
  newBack->value = getLidarDistance(LIDAR_BACK);
  insertAndPop(newBack, &back);

  // average the readings
  frontDist = returnAverage(front);
  backDist = returnAverage(back);
  
  /*if (abs(frontDist - backDist) <= 3)
  {
    deltaFrontBack = 0;
  }
  else
  {
    if (frontDist - backDist <=-3)
    {
      deltaFrontBack = frontDist - backDist + 3;
    }
    else
    {
      deltaFrontBack = frontDist - backDist - 3;
    }
   }*/

   deltaFrontBack = frontDist - backDist;
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

//WE CALL THIS INSTEAD
int getLidarDistance(int sensor)
{
  digitalWrite(LIDAR_FRONT, LOW);
  digitalWrite(LIDAR_BACK, LOW);

  digitalWrite(sensor, HIGH);
  delay(10); // delay for turning on sensor

  return lidarGetRange();
}

/* Convert degree value to radians */
double degToRad(double degrees){
  return (degrees * 71) / 4068;
}

/* Convert radian value to degrees */
double radToDeg(double radians){
  return (radians * 4068) / 71;
}

void getActual()
{
  thetaActual = atan ( (deltaFrontBack)/lengthbetweensensors);
  distanceActual = frontDist * cos (thetaActual);


  Serial.print("\n\nfrontDist-backDist/20: ");
  Serial.print((frontDist-backDist)/lengthbetweensensors);
  
  Serial.print("\n\nTheta Actual: ");
  Serial.print(thetaActual);

  Serial.print("\n\nDistance Actual: ");
  Serial.print(distanceActual);
  
}

void getError()
{
  distanceError = distanceDesired - distanceActual;
  //thetaTurn = thetaMax * (distanceError/maxError);
  if (distanceError >= maxError)
  {
    thetaTurn = -pi/16; //steer right all the way!
  }
  else if (distanceError <= minError)
  {
    thetaTurn = pi/16; //steer left all the way!
  }
  else
  {
    thetaTurn = -1*thetaMax * (distanceError/maxError);
  }
  
  Error = thetaActual + thetaTurn; //softer steer dependent on how large the Error is
 

  Serial.print("\n\nThetaTurn: ");
  Serial.print(thetaTurn);
  
  Serial.print("\n\nError: ");
  Serial.print(Error);
}

void PID() //THIS WILL GIVE YOU 'OUTPUT' TO THE DRIVESTRAIGHT FUNCTION
{
  //Error = Error - pError;
  Integral = Integral + (Error*dt);
  Derivative = (Error - pError)/dt;
  Output = radToDeg((K_p * Error) + (K_i * Integral) + (K_d * Derivative));
  pError = Error;
  
}

void driveStraight()
{
  esc.write(90 + motorSpeed);

  if (pOutput+Output > 127) 
  {
    pOutput = 127;
  }
  else if (pOutput+Output < 37)
  {
    pOutput = 37; 
  }
  else
  {
    pOutput += Output;
  }
  wheels.write(pOutput);

  
  
  deltaFrontBack_calc();
  
  Serial.print("\nnFront:");
  Serial.print(frontDist);
  Serial.print("\nBack:");
  Serial.print(backDist);
  Serial.print("\n\n\nPID Output:");
  Serial.print(Output);
  Serial.print("\nPID pOutput:");
  Serial.print(pOutput);

  Serial.print("\n Count:");
  Serial.print(getCount(front));
}
 
void loop()
{
   // oscillate();
   
   if (startup)
   {
    getActual();
    getError();
    PID(); 
   
    driveStraight();
   }
   delay(50);
}

