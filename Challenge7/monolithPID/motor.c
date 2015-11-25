#include "motor.h"

void initMotor(){
	pinMode(ESC_PIN, PWM_OUTPUT);
	pinMode(WHEEL_PIN, PWM_OUTPUT);
	
	pwmWrite(ESC_PIN, 180);
	sleep(1);
	pwmWrite(ESC_PIN, 0);
	sleep(1);
	pwmWrite(ESC_PIN, NEUTRAL_SPEED);
	sleep(1);
	pwmWrite(ESC_PIN, NEUTRAL_SPEED);
	pwmWrite(WHEEL_PIN, NEUTRAL_ANGLE);
}

void writeAngle(int angle){
	if(angle > 135){
		pwmWrite(WHEEL_PIN, 135);
	} else if (angle < 45){
		pwmWrite(WHEEL_PIN, 45);
	} else {
		pwmWrite(WHEEL_PIN, angle);
	}
}

void writeSpeed(int velocity){
	pwmWrite(ESC_PIN, velocity);
}