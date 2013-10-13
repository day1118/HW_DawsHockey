#include <Servo.h>

#include "config.h"

Servo servoK;
Servo servoF;
Servo servoB;

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

    servoB.write(SERVO_BACK_UP);
}

void loop(){

	servoK.write(SERVO_KICK_UP);

		delay(1000);

		servoB.write(SERVO_BACK_DOWN - 10);


}