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
	wiringPiSetup();
	signal(SIGINT, quit);
	float tempf, tempb;
	
	Lidar* lidar = new Lidar(argv[1], 0, 1);
	lidar->calculateAverages(&tempf, &tempb);
	printf("front: %f back: %f \n", tempf, tempb);
	
	sleep(10);
	thread* lidarThread = lidar->run();
	
	keeprunning = true;
	while(keeprunning){
		lidar->calculateAverages(&tempf, &tempb);
		printf("front: %f back: %f \n", tempf, tempb);
		printf("Distance: %d\n", lidar->getDistance());
		usleep(100000);
	}
	
	lidar->quit();
	lidarThread->join();
	
	delete lidar;
	
	return 0;
}