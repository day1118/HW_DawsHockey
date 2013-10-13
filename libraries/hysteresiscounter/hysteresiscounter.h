#include "Arduino.h"
class HystCount{
public:
	HystCount(int size, int min, int max);
	void change(int val);
	int get_count();
	int on();
	unsigned long time_since_high();
	unsigned long time_since_low();

private:
	int _val;
	int _size;
	int _min;
	int _max;
	int _active;
	unsigned long _lowtime;
	unsigned long _hightime;
};