#include <signal.h>
#include "lidar.h"

bool keeprunning = false;

void quit(int sig){
	keeprunning = false;
}

int main(int argc, char* argv[]){
	if(argc < 2){
		printf("bad usaged\n");
		return -1;
	}
	
	signal(SIGINT, quit);
	
	Lidar* lidar = new Lidar(argv[1], 0, 1);
	thread* lidarThread = lidar->run();
	
	keeprunning = true;
	float tempf, tempb;
	while(keeprunning){
		lidar->calculateAverages(&tempf, &tempb);
		printf("front: %f back: %f \n", tempf, tempb);
		usleep(50000);
	}
	
	lidar->quit();
	lidarThread->join();
	
	delete Lidar;
	
	return 0;
}