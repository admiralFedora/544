#ifndef LIDAR
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/i2c-dev.h>
#include <wiringPi.h>

#define LIDAR

#define DEF_ADDR 0x62

//register addresses
#define CONTROL_REG 0x00
#define STATUS_REG 0x01
#define AQUISIT_REG_MSB 0x0f
#define AQUISIT_REG_LSB 0x10
#define SERIAL_REG 0x96

// commands
#define RESET_FPGA 0x00
#define AQUISIT_DC 0x04
#define AQUISIT_NODC 0x03

// status register bits
#define MEASURE_ERROR 0x40
#define DEVICE_OKAY 0x20
#define INVALID_SIGNAL 0x08
#define DEVICE_READY 0x01

// sensor power pin
#define SENSOR_BACK 17
#define SENSOR_FRONT 18

#define SENSOR_DISTANCE 20 // distance between each sensor in cm

int openDevice(char* filename);
int getDistance(int file);
int readSensor(int location, int file);

#endif
