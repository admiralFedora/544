#include "lidar.h"

Lidar::Lidar(char* filename, int front, int back){
	this->front = front;
	this->back = back;
	
	this->file = open(filename, O_RDWR);
	ioctl(this->file, I2C_SLAVE_FORCE, DEF_ADDR);
	
	pinMode(this->front, OUTPUT);
	pinMode(this->back, OUTPUT);
	
	digitalWrite(this->front, LOW);
	digitalWrite(this->back, LOW);
	
	usleep(10000);
	
	for(int i = 0; i < boxCarLength; i++){
		swapSensors(FRONT);
		frontReadings.push_front(getDistance());
		swapSensors(BACK);
		backReadings.push_front(getDistance());
	}
	
	this->keepRunning = false;
	printf("Front: %d Back:%d\n", this->front, this->back);
}

Lidar::~Lidar(){
	digitalWrite(this->front, LOW);
	digitalWrite(this->back, LOW);
	
	close(this->file);
}

thread* Lidar::run(){
	this->keepRunning = true;
	return new thread(&Lidar::getNewReadings, this);
}

void Lidar::quit(){
	this->keepRunning = false;
}

float Lidar::getWallDistance(){
	float frontAverage;
	float backAverage;
	
	calculateAverages(&frontAverage, &backAverage);
	
	float angleFromWall = atan((frontAverage - backAverage) / sensorDistance);
	
	return frontAverage * cos(angleFromWall);
}

float Lidar::getSensorDifference(){
	float frontAverage;
	float backAverage;
	
	calculateAverages(&frontAverage, &backAverage);
	
	return frontAverage - backAverage;
}

int Lidar::getSensorDistance(){
	return sensorDistance;
}

void Lidar::getNewReadings(){
	while(keepRunning){
		swapSensors(FRONT);
		int front = getDistance();
		swapSensors(BACK);
		int back = getDistance();
		
		readings.lock();
		if(front > 0){
			frontReadings.pop_back();
			frontReadings.push_front(front);
		}
		if(back > 0){
			backReadings.pop_back();
			backReadings.push_front(back);
		}
		readings.unlock();
	}
}

int Lidar::getDistance(){
	i2c_smbus_write_byte_data(this->file, CONTROL_REG, RESET_FPGA);
	usleep(20000); // wait for the fpga to reset 
	i2c_smbus_write_byte_data(this->file, CONTROL_REG, AQUISIT_DC);
	usleep(20000); // for data to be aquired and written to the appropriate registers
	
	return (i2c_smbus_read_byte_data(file, AQUISIT_REG_MSB) << 8) | i2c_smbus_read_byte_data(file, AQUISIT_REG_LSB);
}

void Lidar::swapSensors(int sensor){
	if(FRONT == sensor){
		digitalWrite(this->front, HIGH);
		digitalWrite(this->back, LOW);
	} else if(BACK == sensor){
		digitalWrite(this->front, LOW);
		digitalWrite(this->back, HIGH);
	}
	
	usleep(10000);
}

void Lidar::calculateAverages(float* frontAverage, float* backAverage){
	readings.lock();
	deque<int>::iterator frontIt = frontReadings.begin();
	deque<int>::iterator backIt = backReadings.begin();
	while(frontIt != frontReadings.end()){
		*frontAverage += *frontIt++;
	}
	while(backIt != backReadings.end()){
		*backAverage += *backIt++;
	}
	readings.unlock();
	
	*frontAverage /= (float) boxCarLength;
	*backAverage /= (float) boxCarLength;
}