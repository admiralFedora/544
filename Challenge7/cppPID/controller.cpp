#include "Controller.h"

Controller::Controller(char* filename, int escPin, int wheelPin,float distanceDeisre, float delay, float kp, float ki, float kd){
	this->distanceDeisre = distanceDesire;
	this->delay = delay;
	this->pError = 0f;
	this->pOutput = 0f;
	this->integral = 0f;
	this->derivative = 0f;
	this->kp = kp;
	this->ki = ki;
	this->kd = kd;
	
	this->lidar = new Lidar(filename);
	this->esc = new Motor(escPin);
	this->wheel = new Wheel(wheelPin);
	
	this->initSys();
}

Controller::~Controller(){
	esc.write(STOP); // stop moving the car
	wheel.write(CENTERPOINT);
	
	delete esc;
	delete wheel;
	delete lidar;
}

pthread_t Controller::run(){
	pthread_t ret;
	this->keepRunning = true;
	pthread_create(&ret, NULL, (void*) &PID, NULL);
	return ret;
}

void Controller::quit(int sig){
	this->keepRunning = false;
}

void Controller::PID(void* arg){
	signal(SIGUSR1, Controller::quit);
	while(this->keepRunning){
		float wallDistance = lidar->getWallDistance();
		float distRatio = wallDistance / distanceDeisre;
		float distError = distanceDeisre - wallDistance;
		float heading = atan(lidar->getSensorDifference / lidar->getSensorDistance);
		
		float error = -1 * atan(distError/distanceDesire);
		
		if(distRatio < 1){
			error = (.1 * heading) + (1/distRatio) * error;
		} else {
			error = (.1 * heading) + (distRatio) * error;
		}
		
		this->integral = this->integral + (error * this->delay);
		this->derivative = (error - this->pError) / this->delay;
		
		float output = radToDeg( (kp * error) + (ki * this->integral) + (kd * this->derivative));
		
		esc.write(CENTERPOINT + MOTORSPEED);
		output += CENTERPOINT;
		wheel.write(output);
		usleep(this->delay*1000);
	}
}

void Controller::initSys(){
	esc.write(180);
	sleep(1);
	esc.write(0);
	sleep(1);
	esc.write(STOP);
	sleep(1);
	
	wheel.write(CENTERPOINT);
}

float Controller:radToDeg(float rad){
	return (rad * 4068) / 71;
}