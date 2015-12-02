#include "motor.h"

Motor::Motor(int pin){
	this->pin = pin;
}

void Motor::write(int value){
	if(value > 135){
		gpioServo(pin, angleToPulseWidth(135));
	} else if(value < 45){
		gpioServo(pin, angleToPulseWidth(45));
	} else {
		gpioServo(pin, angleToPulseWidth(value));
	}
}

void Motor::writeNoSafe(int value){
	gpioServo(pin, angleToPulseWidth(value));
}

int Motor::angleToPulseWidth(int value){
	return (1000/90)*value+500;
}