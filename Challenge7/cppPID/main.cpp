#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <pigpio.h>
#include "controller.h"

static Controller* controller;

void quit(int sig){
	printf("SIGINT captured\n");
	controller->quit();
}

int main(int argc, char* argv[]){
	if(argc < 2){
		printf("Usage: %s <i2c file>\n", argv[0]);
		return -1;
	}
	
	gpioInitialise();
	signal(SIGINT, quit);
	bool init = false;
	if(argc > 2 && strcmp(argv[2], "-i")){
		init = true;
	}
	controller = new Controller(argv[1], true);
	
	controller->run()->join();
	delete controller;
	gpioTerminate();
	printf("Quit was sucessful\n");
	return 0;
}