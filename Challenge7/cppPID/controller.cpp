#include "controller.h"

Controller::Controller(char* filename, bool runInit, int escPin, int wheelPin,float distanceDesire, float delay, float kp, float ki, float kd){
	this->distanceDesire = distanceDesire;
	this->delay = delay;
	this->pError = 0;
	this->pOutput = 0;
	this->integral = 0;
	this->derivative = 0;
	this->kp = kp;
	this->ki = ki;
	this->kd = kd;
	
	this->lidar = new Lidar(filename);
	this->esc = new Motor(escPin);
	this->wheel = new Motor(wheelPin);
	
	if(runInit){
		this->initSys();
	}
	
	this->lidarThread = this->lidar->run();
}

Controller::~Controller(){
	esc->write(STOP); // stop moving the car
	wheel->write(CENTERPOINT);
	lidar->quit();
	lidarThread->join();
	
	delete esc;
	delete wheel;
	delete lidar;
}

thread* Controller::run(){
	this->keepRunning = true;
	return new thread(&Controller::PID, this);
}

void Controller::quit(){
	this->keepRunning = false;
}

void Controller::PID(){
	while(this->keepRunning){
		float wallDistance = lidar->getWallDistance();
		float distRatio = wallDistance / distanceDesire;
		float distError = distanceDesire - wallDistance;
		float heading = atan(lidar->getSensorDifference() / lidar->getSensorDistance());
		
		float error = -1 * atan(distError/distanceDesire);
		
		if(distRatio < 1){
			error = (.1 * heading) + (1/distRatio) * error;
		} else {
			error = (.1 * heading) + (distRatio) * error;
		}
		
		this->integral = this->integral + (error * this->delay);
		this->derivative = (error - this->pError) / this->delay;
		
		float output = radToDeg( (kp * error) + (ki * this->integral) + (kd * this->derivative));
		
		esc->write(STOP + MOTORSPEED);
		output += CENTERPOINT;
		wheel->write(output);
		usleep(this->delay*1000);
	}
}

void Controller::initSys(){
	esc->writeNoSafe(180);
	sleep(1);
	esc->writeNoSafe(0);
	sleep(1);
	esc->writeNoSafe(STOP);
	sleep(1);
	
	wheel->write(CENTERPOINT);
}

float Controller::radToDeg(float rad){
	return (rad * 4068) / 71;
}
