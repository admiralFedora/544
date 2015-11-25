#include <stdio.h>
#include "lidar.h"
#include "motor.h"
#include "Fifo.h"

void init(){
	wiringPiSetupGpio(); 
}

int main(int argc, char* argv[]){
	int file;
	if(argc < 2){
		printf("need to specify i2c file\n");
		return -1;
	}
	file = openDevice(argv[1]);
	if(file != 0){
		printf("could not open i2c file \n");
		return -1;
	}
	
	return 0;
}