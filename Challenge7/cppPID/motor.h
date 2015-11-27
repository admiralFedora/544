#ifndef MOTOR
#define MOTOR

#include <wiringPi.h>

using namespace std;

class Motor{
public:
	Motor(int pin);
	void write(int value);
private:
	int pin;
};

#endif