#include <Servo.h>
#include <Wire.h>
#include <math.h>
#include "Fifo.h"
#include <SimpleTimer.h>

#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.
#define    pi 3.14159

#define LIDAR_FRONT 5
#define LIDAR_BACK 6
#define SONAR_PIN 7
#define XBEE_PIN 4
#define WARNING_PIN 12

#define K_p .8
#define K_i 0.0
#define K_d 525.2
#define dt 160
#define lengthbetweensensors 20.0 //in centimeters, must be the same unit as getLidarDistance
#define centerpoint 82 //CALIBRATED CENTER
#define motorSpeed -10

Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
volatile bool startup = true; // used to ensure startup only happens once
volatile bool startRun = true;
int frontDist = 0; 
int backDist = 0;
int deltaFrontBack = 0;

int Output = 82; //OUTPUT
int pOutput = 82; // PREVIOUS OUTPUT

float thetaActual;
float distanceDesired;
float distanceActual;
float thetaTurn;
float thetaMax = pi/32;

float Error = 0.0;
float pError = 0.0;
float distanceError;
float thetaError;
float maxError;
float minError;
float Integral;
float Derivative;

Fifo *front, *back;

SimpleTimer timer;
int counter = 0;

// wheel angle > 90 turns left (facing forward)
// wheel angle < 90 turns right (facing forward)

// speed > 90 is backwards (steering in front)
// speed < 90 is forwards (steering in front)
 
void setup()
{
  Serial.begin(9600);
  //pinMode(2, INPUT);
  Serial.println("< START >");
  
  wheels.attach(8); // initialize wheel servo to Digital IO Pin #8
  esc.attach(9); // initialize ESC to Digital IO Pin #9
  /*  If you're re-uploading code via USB while leaving the ESC powered on, 
   *  you don't need to re-calibrate each time, and you can comment this part out.
   */
  calibrateESC();

  // LIDAR control
  Wire.begin(); // join i2c bus
  
  pinMode(LIDAR_FRONT, OUTPUT);
  pinMode(LIDAR_BACK, OUTPUT);
  pinMode(WARNING_PIN, OUTPUT);
  pinMode(SONAR_PIN, INPUT_PULLUP);
  pinMode(XBEE_PIN, INPUT_PULLUP);

  digitalWrite(LIDAR_FRONT, LOW);
  digitalWrite(LIDAR_BACK, LOW);

  distanceDesired = 60.0;

  initReadings(LIDAR_FRONT, &front);
  initReadings(LIDAR_BACK, &back);

  deltaFrontBack_calc();
    

  //distanceDesired = getLidarDistance(LIDAR_FRONT);
  
  maxError = 1.0 * distanceDesired;
  minError = -1.0 * distanceDesired;

  timer.setInterval(50, driveCar);
}

void initReadings(int sensor, Fifo **fifo){
  int i;
  Fifo *temp;
  *fifo = (Fifo*) malloc(sizeof(Fifo));
  (*fifo)->value = distanceDesired;
  (*fifo)->next = NULL;
  for(i = 0; i < 4; i++){
    temp = (Fifo*) malloc(sizeof(Fifo));
    temp->value = distanceDesired;
    temp->next = *fifo;
    *fifo = temp;
  }
}

bool shouldRun(){
  if(digitalRead(SONAR_PIN) == LOW){
    Serial.println("stopped");
    return false;
  } else {
    return true;
  }
}

bool shouldStart(){
  if(digitalRead(XBEE_PIN) == LOW){
    return false;
  } else {
    return true;
  }
}

void deltaFrontBack_calc()
{
  // get one new reading and remove the oldest
  Fifo *newFront = (Fifo*) malloc(sizeof(Fifo));
  newFront->value = getLidarDistance(LIDAR_FRONT);
  if(newFront->value <= (returnAverage(front)+150)){
    insertAndPop(newFront, &front);
  }

  Fifo *newBack = (Fifo*) malloc(sizeof(Fifo));
  newBack->value = getLidarDistance(LIDAR_BACK);
  if(newBack->value <= (returnAverage(back)+150)){
    insertAndPop(newBack, &back);
  }

  // average the readings
  frontDist = returnAverage(front);
  backDist = returnAverage(back);
  
  /*if (abs(frontDist - backDist) <= 10)
  {
    deltaFrontBack = 0;
  }
  else
  {
    if (frontDist - backDist <=-10)
    {
      deltaFrontBack = frontDist - backDist + 10;
    }
    else
    {
      deltaFrontBack = frontDist - backDist - 10;
    }
   }*/

   deltaFrontBack = frontDist - backDist;
}

/* Calibrate the ESC by sending a high signal, then a low, then middle.*/
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

  Serial.print("\n\nTime:");
  Serial.print(counter);

  Serial.print("\nFront:");
  Serial.print(frontDist);
  Serial.print("\nBack:");
  Serial.print(backDist);

  Serial.print("\nTheta Actual: ");
  Serial.print(thetaActual);

  Serial.print("\nDistance Actual: ");
  Serial.print(distanceActual); 
}

void getError()
{
  float distanceRatio = distanceActual/distanceDesired;
  distanceError = distanceDesired - distanceActual;
  float triangleTheta = -1 * atan(distanceError/distanceDesired);
  thetaTurn = triangleTheta;
  /*if(triangleTheta < 0){
    thetaTurn = -1 * (-(pi/2) - triangleTheta);
  } else if (triangleTheta > 0) {
    thetaTurn = -1 * ((pi/2) - triangleTheta);
  } else {
    thetaTurn = 0;
  }*/
  //thetaTurn = -1 * (90 - atan(distanceDesired/distanceError));
  /*if (distanceError >= maxError)
  {
    thetaTurn = -thetaMax; //steer right all the way!
  }
  else if (distanceError <= minError)
  {
    thetaTurn = thetaMax; //steer left all the way!
  }
  else
  {
    thetaTurn = -(1/distanceRatio)*thetaMax * (distanceError/maxError);
  }*/

  //thetaTurn = -1*thetaMax * (distanceError/maxError);

  if(distanceRatio < 1){
    Error = (.1 * thetaActual) + (1/distanceRatio) * thetaTurn; //softer steer dependent on how large the Error is
  } else {
    Error = (.1 * thetaActual) + (distanceRatio) * thetaTurn; //softer steer dependent on how large the Error is
  }
   
  Serial.print("\nDistanceRatio:");
  Serial.print(distanceRatio);
  
  Serial.print("\nDistanceError:");
  Serial.print(distanceError);

  Serial.print("\nThetaTurn: ");
  Serial.print(thetaTurn);
  
  Serial.print("\nError: ");
  Serial.print(Error);
}

void PID() //THIS WILL GIVE YOU 'OUTPUT' TO THE DRIVESTRAIGHT FUNCTION
{
  //Error = Error - pError;
  Integral = Integral + (Error*dt);
  Derivative = (Error - pError)/dt;
  //Derivative = (Error - returnAverage(bError))/dt;
  Output = radToDeg((K_p * Error) + (K_i * Integral) + (K_d * Derivative));
  pError = Error;

  /*Fifo *newError = (Fifo*) malloc(sizeof(Fifo));
  newError->value = Error;
  insertAndPop(newError, &bError);*/
  
}

void driveStraight()
{
  esc.write(centerpoint + motorSpeed);

  pOutput = centerpoint + Output;
  if ((pOutput) >= 135) 
  {
    pOutput = 135;
    digitalWrite(WARNING_PIN, HIGH);
  }
  else if ((pOutput) <= 45)
  {
    pOutput = 45;
    digitalWrite(WARNING_PIN, HIGH);
  } else {
    digitalWrite(WARNING_PIN, LOW);
  }

  //pOutput += Output;
  wheels.write(pOutput);
  
  deltaFrontBack_calc();

  Serial.print("\nPID Output:");
  Serial.print(Output);
  Serial.print("\nPID pOutput:");
  Serial.print(pOutput);
}

void driveCar()
{
   if (startRun && startup)
   {
    getActual();
    getError();
    PID(); 
   
    driveStraight();
    counter++;
   } else {
    esc.write(90);
   }
}
 
void loop()
{
   //driveCar();
   //startRun = shouldStart();
   //startup = shouldRun();
   //delay(50);
   timer.run();
}

