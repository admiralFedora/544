#include <wiringPi.h>

#define ESC_PIN 19
#define WHEEL_PIN 20
#define NEUTRAL_ANGLE 82
#define NEUTRAL_SPEED 90

void initMotor();
void writeAngle(int angle);
void writeSpeed(int velocity);