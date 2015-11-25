#include <stdio.h>
#include <math.h>
#include "lidar.h"
#include "motor.h"

typedef struct {
	int i2cfile;
	float distanceDesired;
	int delay;
} ControlStruct;

int controllerInit(ControlStruct* control);
float distanceFromWall(ControlStruct* control);
float calculateError(ControlStruct* control, float wallDistance);