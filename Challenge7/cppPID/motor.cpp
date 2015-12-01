#include "motor.h"

Motor::Motor(int pin){
	this->pin = pin;
	pinMode(pin, PWM_OUTPUT);
}

void Motor::write(int value){
	if(value > 135){
		pwmWrite(pin, 135);
	} else if(value < 45){
		pwmWrite(pin, 45);
	} else {
		pwmWrite(pin, value);
	}
}

void Motor::writeNoSafe(int value){
	pwmWrite(pin, value);
}