#ifndef MOTOR
#define MOTOR

#include <pigpio.h>

using namespace std;

class Motor{
public:
	Motor(int pin);
	void write(int value); // function with safety margins for wheel
	void writeNoSafe(int value); // no safety margin, use with caution you could break the motors
private:
	int pin;
	
	int angleToPulseWidth(int value);
};

#endif