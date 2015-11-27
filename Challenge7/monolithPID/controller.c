#include "controller.h"

int controllerInit(ControlStruct* control){
	control = (ConstructStruct*) malloc(sizeof(ControlStruct));
	control->file = openDevice(argv[1]);
	control->distanceDesired = 70;
	control->delay = 50;
	control->front = NULL;
	control->back = NULL;
	control->pError = 0.0;
	control->pOutput = 0.0;
	control->integral = 0.0;
	control->derivative = 0.0;
	
	if(control->file){
		printf("could not open i2c file\n");
		return -1;
	}
	
	return 0;
}

void initReadings(ControlStruct* control){
	int i;
	for(i = 0; i < 5; i++){
		Fifo* front = (Fifo*) malloc(sizeof(Fifo));
		Fifo* back = (Fifo*) malloc(sizeof(Fifo));
		
		front->value = readSensor(SENSOR_FRONT, control->file);
		back->value = readSensor(SENSOR_BACK, control->file);
		
		insert(front, &(control->front));
		insert(back, &(control->back));
	}
}



float distanceFromWall(ControlStruct* control){
	float frontSensor = readSensor(SENSOR_FRONT, control->file);
	float backSensor = readSensor(SENSOR_BACK, control->file);
	
	float angleFromWall = atan( (frontSensor - backSensor) / SENSOR_DISTANCE);
	
	return frontSensor * cos(angleFromWall);
}

float calculateError(ControlStruct* control){
	float distRatio = distanceActual(control) / DESIRE_DISTANCE;
	float distError = DESIRE_DISTANCE - distanceActual(control);
	
	float thetaError = -1 * atan(distError/DESIRE_DISTANCE);
	
	if(distanceRatio < 1){
		return (.1 * thetaActual) + (1/distRatio) * thetaTurn;
	} else {
		return (.1 * thetaActual) + distanceRatio * thetaTurn;
	}
}

void PID(ControlStruct* control){
	float error = calculateError(control);
	
	control->integral  = control->integral + error* DELAY;
	control->derivative = (error - control->pError)/DELAY;
}