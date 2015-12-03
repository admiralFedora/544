#include <pigpio.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include "motor.h"

int main(){
	gpioInitialise();
	gpioSetPWMfrequency(22, 50);
	gpioSetPWMrangee(22, 5000);
	int val;
	while(1){
		cin >> val;
		gpioPWM(22, val);
	}
	
	gpioTerminate();
	return 0;
}