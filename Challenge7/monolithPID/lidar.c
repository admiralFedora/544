#include <unistd.h>
#include "lidar.h"

int openDevice(char* filename){
  int file;

  if((file = open(filename, O_RDWR)) < 0){
    return -1;
  }

  if((ioctl(file, I2C_SLAVE_FORCE, DEF_ADDR)) < 0){
    close(file);
    return -1;
  }
  
  pinMode(SENSOR_FRONT, OUTPUT);
	pinMode(SENSOR_BACK, OUTPUT);
  // turn off both sensors
  digitalWrite(SENSOR_FRONT, LOW);
  digitalWrite(SENSOR_BACK, LOW);
  return file;
}

int getDistance(int file){
  
  i2c_smbus_write_byte_data(file, CONTROL_REG, RESET_FPGA);
  usleep(20000); // wait for the fpga to reset 
  i2c_smbus_write_byte_data(file, CONTROL_REG, AQUISIT_DC);
  usleep(20000); // for data to be aquired and written to the appropriate registers

  return (i2c_smbus_read_byte_data(file, AQUISIT_REG_MSB) << 8) | i2c_smbus_read_byte_data(file, AQUISIT_REG_LSB);
}

int readSensor(int location, int file){
  if(location == SENSOR_FRONT){
    digitalWrite(SENSOR_FRONT, HIGH);
    digitalWrite(SENSOR_BACK, LOW);
  } else {
    digitalWrite(SENSOR_FRONT, LOW);
    digitalWrite(SENSOR_BACK, HIGH);
  }
  
  usleep(10000); // wait for the sensors to reactivate
  
  return getDistance(file);
}
