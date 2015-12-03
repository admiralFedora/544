GPP=g++
LIBS=-lpigpio -lpthread -lrt
PPFLAGS=-std=c++11 $(LIBS)

all: main.o controller.o lidar.o motor.o
	$(GPP) -s $^ $(PPFLAGS) -o straight
	
testLidar: testLidar.o lidar.o
	$(GPP) -s $^ $(PPFLAGS) -o $@
	
testMotor: testMotor.o controller.o
	$(GPP) -s $^ $(PPFLAGS) -o $@

main.o: main.cpp
	$(GPP) -c $^ $(PPFLAGS) -o $@
	
controller.o: controller.cpp
	$(GPP) -c $^ $(PPFLAGS) -o $@
	
lidar.o: lidar.cpp
	$(GPP) -c $^ $(PPFLAGS) -o $@
	
motor.o: motor.cpp
	$(GPP) -c $^ $(PPFLAGS) -o $@
	
testLidar.o: testLidar.cpp
	$(GPP) -c $^ $(PPFLAGS) -o $@
	
testMotor.o: testMotor.cpp
	$(GPP) -c $^ $(PPFLAGS) -o $@
	
clean:
	rm -rf *.o straight testLidar
