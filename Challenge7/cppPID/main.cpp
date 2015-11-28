#include <stdio.h>
#include "controller.h"

static Controller* controller;

void quit(int sig){
	printf("SIGINT captured\n");
	controller->quit();
}

int main(int argc, char* argv[]){
	if(argc != 2){
		printf("Usage: %s <i2c file>\n", argv[0]);
		return -1;
	}
	
	controller = new Controller(argv[1]);
	
	controller->run()->join();
	delete controller;
	
	printf("Quit was sucessful\n");
	return 0;
}