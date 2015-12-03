#include "motor.h"

int main(){
	gpioInitialise();
	Motor* esc = new Motor(27);
	Motor* wheel = new Motor(22);
	
	esc->writeNoSafe(180);
	sleep(1);
	esc->writeNoSafe(0);
	sleep(1);
	esc->writeNoSafe(STOP);
	sleep(1);
	
	wheel->write(CENTERPOINT);
	
	esc->write(80);
	wheel->write(100);
	
	sleep(10);
	
	esc->write(90);
	wheel->write(90);
	
	return 0;
}