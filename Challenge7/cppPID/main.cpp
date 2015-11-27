#include <stdio.h>

void quit(int sig){
	printf("SIGINT captured\n");
	raise(SIGUSR1);
}

int main(int argc, char* argv[]){
	if(argc != 2){
		printf("Usage: %s <i2c file>\n", argv[0]);
		return -1;
	}
	
	Controller* controller = new Controller(argv[1]);
	
	signal(SIGINT, quit);
	pthread_join(controller->run(), NULL);
	delete controller;
	
	printf("Quit was sucessful\n");
	return 0;
}