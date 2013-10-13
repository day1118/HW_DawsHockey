#include "motors.h"

Motor::Motor(int apin, int bpin, int epin){
	_apin = apin;
	_bpin = bpin;
	_epin = epin;
	_pwm = 0;

	pinMode(_apin, OUTPUT);
	pinMode(_epin, OUTPUT);
	pinMode(_bpin, OUTPUT);

	digitalWrite(_apin, 0);
	digitalWrite(_bpin, 0);

}

void Motor::brake(){
	digitalWrite(_apin, 0);
	digitalWrite(_bpin, 0);
}

void Motor::drivef(int pwm){
	int _pwm = pwm;
#ifdef MOTORS_ON
	analogWrite(_epin, _pwm);
	digitalWrite(_apin, 1);
	digitalWrite(_bpin, 0);
#endif
}

void Motor::driveb(int pwm){
	int _pwm = pwm;
#ifdef MOTORS_ON
	analogWrite(_epin, _pwm);
	digitalWrite(_apin, 0);
	digitalWrite(_bpin, 1);
#endif
}