#include "controller.h"

int controllerInit(ControlStruct* control){
	control = (ConstructStruct*) malloc(sizeof(ControlStruct));
	control->file = openDevice(argv[1]);
	control->distanceDesired = 70;
	control->delay = 50;
	
	if(control->file){
		printf("could not open i2c file\n");
		return -1;
	}
	
	return 0;
}

float distanceFromWall(ControlStruct* control){
	float frontSensor = readSensor(SENSOR_FRONT, control->file);
	float backSensor = readSensor(SENSOR_BACK, control->file);
	
	float difference = 
}