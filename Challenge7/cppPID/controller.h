#ifndef CONTROLLER
#define CONTROLLER

#include <pthread.h>
#include <signal.h>
#include "motor.h"
#include "lidar.h"

using namespace std;

#define STOP		90
#define CENTERPOINT 82
#define MOTORSPEED -10

class Controller{
public:
	Controller(char* filename, int escPin = 15, int wheelPin = 16, float distanceDeisre = 70, float delay = 50, float kp = 0.8, float ki = 0.0, float kd = 525.2);
	~Controller();
	pthread_t run();
private:
	Lidar* lidar;
	Motor* esc;
	Motor* wheel;
	float distanceDesire;
	float delay;
	float kp;
	float ki;
	float kd;
	float pError;
	float pOutput;
	float integral;
	float derivative;
	bool keepRunning;
	
	void quit(int sig);
	void PID(void* arg);
	void initSys();
	float radToDeg(rad);
};

#endif