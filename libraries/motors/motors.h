#include "Arduino.h"
#include "config.h"

class Motor
{

public:
	Motor(int apin, int bpin, int epin);
	void brake();
	void drivef(int pwm);
	void driveb(int pwm);

private:
	int _apin;
	int _bpin;
	int _epin;
	int _pwm;
};