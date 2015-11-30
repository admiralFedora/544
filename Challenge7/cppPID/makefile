GPP=g++
PPFLAGS=-std=c++11 -lwiringPi -lpthread

all: main.o controller.o lidar.o motor.o
	$(GPP) -s $^ $(PPFLAGS) -o straight

main.o: main.cpp
	$(GPP) -c $^ $(PPFLAGS) -o $@
	
controller.o: controller.cpp
	$(GPP) -c $^ $(PPFLAGS) -o $@
	
lidar.o: lidar.cpp
	$(GPP) -c $^ $(PPFLAGS) -o $@
	
motor.o: motor.cpp
	$(GPP) -c $^ $(PPFLAGS) -o $@
	
clean:
	rm -rf *.o straight
