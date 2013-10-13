#include "config.h"
#include "motors.h"

Motor motorl(MOTOR_L_A_PIN,
	MOTOR_L_B_PIN,
	MOTOR_L_ENABLE_PIN);

Motor motorr(MOTOR_R_A_PIN,
	MOTOR_R_B_PIN,
	MOTOR_R_ENABLE_PIN);

int speed = 255;

void setup(){
	Serial.begin(9600);


}

void loop(){

	motorl.brake();
	motorr.drivef(speed);
	delay(1000);
	motorr.driveb(speed);

	delay(1000);

	motorr.brake();

	delay(1000);
	motorl.drivef(speed);
	delay(1000);
	motorl.driveb(speed);
	delay(1000);

	motorl.drivef(speed);
	motorr.drivef(speed);

	delay(2000);

	motorl.driveb(speed);
	motorr.driveb(speed);
	delay(2000);
}