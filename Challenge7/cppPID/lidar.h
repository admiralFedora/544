#ifndef LIDAR
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/i2c-dev.h>
#include <pigpio.h>
#include <deque>
#include <math.h>
#include <unistd.h>
#include <mutex>
#include <thread>

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

#define FRONT 0
#define BACK 1

using namespace std;

class Lidar{
public:
	Lidar(char* filename, int front = 17, int back = 18);
	~Lidar();
	thread* run();
	void quit();
	float getWallDistance();
	float getSensorDifference();
	int getSensorDistance();
	void calculateAverages(float* frontAverage, float* backAverage);
	int getDistance();
		
	mutex readings;
private:
	deque<int> frontReadings;
	deque<int> backReadings; 
	int front;
	int back;
	int sensorDistance = 20;
	int file;
	bool keepRunning;
	
	const int boxCarLength = 5;
	
	void getNewReadings();
	void swapSensors(int sensor);
};

#endif
