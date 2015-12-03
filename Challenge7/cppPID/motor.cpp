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
	float temp = (1500.0/90.0)*value;
	return (int) (temp+500);
}