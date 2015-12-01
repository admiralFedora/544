#include <signal.h>
#include "lidar.h"

bool keeprunning = false;

void quit(){
	keeprunning = false;
}

int main(int argc, char* argv[]){
	if(argc < 2){
		printf("bad usaged\n");
		return -1;
	}
	
	signal(SIGINT, quit);
	
	Lidar* lidar = new Lidar(argv[1]);
	thread* lidarThread = lidar->run();
	
	keeprunning = true;
	float temp;
	while(keeprunning){
		temp = lidar->getSensorDifference();
		printf("%f", temp);
	}
	
	lidar->quit();
	lidarThread->join();
	
	return 0;
}