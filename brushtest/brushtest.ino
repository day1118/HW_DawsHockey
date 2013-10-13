#include "motors.h"
#include "config.h"

Motor motorb(MOTOR_B_A_PIN,
	MOTOR_B_B_PIN,
	MOTOR_B_ENABLE_PIN);

#define DELAY 	4000
void setup(){
	

}

void loop(){
	motorb.drivef(100);

	delay(DELAY);

	motorb.driveb(100);

	delay(DELAY);

	motorb.drivef(200);

	delay(DELAY);

	motorb.driveb(200);

	delay(DELAY);
}