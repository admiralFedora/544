/*
/* Short example based on the Servo library example "Sweep" example and the Pulsed Light 3D examples.
/* The servo motor* is attached to pin 5 and rotates from 0-180 degrees continuously by 1 degree increments.
/* The LIDAR lite is attached to VCC (+5 V DC), GND, SDA and SCL.
/* 
/* This examples uses the Wire library.
/*
/* Hardware used:
/* BotBoarduino: http://www.robotshop.com/en/lynxmotion-botboarduino-robot-controller.html?product-code=RB-Lyn-363
/* LIDAR-Lite Laser Rangefinder: http://www.robotshop.com/en/lidar-lite-laser-rangefinder-pulsedlight.html?product-code=RB-Pli-01
/* HS-645MG servo motor: http://www.robotshop.com/en/hitec-hs645mg-servo-motor.html?product-code=RB-Hit-29
/* (alternative) HS-422 servo motor: http://www.robotshop.com/en/hitec-hs422-servo-motor.html?product-code=RB-Hit-27
/* Bracket to hold the LIDAR: http://www.robotshop.com/en/aluminum-l-connector-bracket-asb-06.html?product-code=RB-Lyn-83
/* or this one: http://www.robotshop.com/en/lynxmotion-multi-purpose-sensor-housing.html?product-code=RB-Lyn-75
/* Bracket to hold the servo motor: http://www.robotshop.com/en/lynxmotion-aluminum-multi-purpose-servo.html?product-code=RB-Lyn-81
/*
/* *: Please note that while we used a HS-645MG in our example, the HS-422 and almost any other standard RC servo motor could perform this task.
*/

#include <Wire.h>

#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.

#define LIDAR_FRONT 2
#define LIDAR_BACK 3

#include <Servo.h>

Servo myservo;

int pos = 0;         // Position of the servo (degress, [0, 180])
int distance = 0;    // Distance measured

void setup()
{
  // Serial output
  Serial.begin(9600);
  Serial.println("< START >");
  
  // LIDAR control
  Wire.begin(); // join i2c bus

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
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

void serialPrintRange(int distance)
{
    Serial.print("Distance (cm): ");
    Serial.println(distance);
}

int getLidarDistance(int sensor){
  digitalWrite(LIDAR_FRONT, LOW);
  digitalWrite(LIDAR_BACK, LOW);

  digitalWrite(sensor, HIGH);
  delay(5); // delay for turning on sensor

  return lidarGetRange();
}

void loop()
{
  Serial.print("Sensor 1:");
  serialPrintRange(getLidarDistance(LIDAR_FRONT));

  Serial.print("Sensor 2:");
  serialPrintRange(getLidarDistance(LIDAR_BACK));

  delay(1000);
}

