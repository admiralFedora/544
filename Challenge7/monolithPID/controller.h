#include <stdio.h>
#include <math.h>
#include "lidar.h"
#include "motor.h"
#include "Fifo.h"

#define DESIRE_DISTANCE 50
#define DELAY 160
#define KP 0.8
#define KI 0.0
#define KD 525.2

typedef struct {
	int i2cfile;
	float distanceDesired;
	int delay;
	Fifo* front;
	Fifo* back;
	float pError;
	float pOutput;
	float integral;
	float derivative;
} ControlStruct;

int controllerInit(ControlStruct* control);
float distanceFromWall(ControlStruct* control);
float calculateError(ControlStruct* control, float wallDistance);
void PID(ControlStruct* control);