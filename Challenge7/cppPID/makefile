GPP=g++
PPFLAGS=`-std=c++11 -stdlib=libc++`

all: main.o controller.o lidar.o motor.o
	GPP PPFLAGS -s $^ -o straight

main.o: main.cpp
	GPP PPFLAGS -c $^ -o $@
	
controller.o: controller.cpp
	GPP PPFLAGS -c $^ -o $@
	
lidar.o: lidar.cpp
	GPP PPFLAGS -c $^ -o $@
	
motor.o: motor.cpp
	GPP PPFLAGS -c $^ -o $@
	
clean:
	rm -rf *.o straight