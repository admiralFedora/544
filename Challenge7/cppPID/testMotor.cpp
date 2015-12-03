#include <pigpio.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include "motor.h"

int main(){
	gpioInitialise();
	Motor* esc = new Motor(27);
	
	esc->writeNoSafe(180);
	sleep(1);
	esc->writeNoSafe(0);
	sleep(1);
	esc->writeNoSafe(90);
	sleep(1);
	
	
	gpioSetPWMfrequency(22, 50);
	gpioSetPWMrange(22, 5000);
	int val;
	while(1){
		cin >> val;
		gpioPWM(22, val);
	}
	
	gpioTerminate();
	return 0;
}