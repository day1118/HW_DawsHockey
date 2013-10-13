#include <Servo.h>
#include "config.h"

Servo servoF;
Servo servoB;
Servo servoK;

int kickStart;

unsigned long kickT1;
unsigned long kickT2;
unsigned long kickT3;
unsigned long kickT4;
unsigned long kickT5;
unsigned long kickT6;
unsigned long kickT7;

void setup(){
	
    servoF.attach(SERVO_FRONT_PIN,
    	SERVO_FRONT_MIN_PWM,
    	SERVO_FRONT_MAX_PWM);
    servoB.attach(SERVO_BACK_PIN,
    	SERVO_BACK_MIN_PWM,
    	SERVO_BACK_MAX_PWM);
    servoK.attach(SERVO_KICK_PIN,
    	SERVO_KICK_MIN_PWM,
    	SERVO_KICK_MAX_PWM);

    Serial.begin(9600);

}

void loop(){
	if (kickStart) {
		int dur = 800;
		kickT1 = millis();
		kickT2 = kickT1 + dur;
		kickT3 = kickT2 + dur;
		kickT4 = kickT3 + dur;
		kickT5 = kickT4 + dur;
		kickT6 = kickT5 + dur;
		kickT7 = kickT6 + dur;
		kickStart = 0;
	}	
	unsigned long time = millis();
	if (time > kickT1 && time < kickT2){
		PLOT("STATE", 1);
		servoF.write(SERVO_FRONT_UP);
	}
	if (time > kickT2 && time < kickT3){
		PLOT("STATE", 2);
		servoB.write(SERVO_BACK_UP);	
	}
	if (time > kickT3 && time < kickT4){
		PLOT("STATE", 3);
		servoK.write(SERVO_KICK_DOWN);
	}
	if (time > kickT4 && time < kickT5){
		PLOT("STATE", 4);
		servoK.write(SERVO_KICK_UP);
	}
	if (time > kickT5 && time < kickT6){
		PLOT("STATE", 5);
		servoF.write(SERVO_FRONT_DOWN);
	}
	if (time > kickT6 && time < kickT7){
		PLOT("STATE", 6);
		servoB.write(SERVO_BACK_DOWN);
	} 
	if (time > kickT7){
		kickStart = 1;
		} 
}