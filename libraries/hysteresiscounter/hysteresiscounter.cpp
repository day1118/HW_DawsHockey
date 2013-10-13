#include "hysteresiscounter.h"

HystCount::HystCount(int size,
	int min, int max){
	_val = 0;
	_active =0;
	_size = size;
	_min = min;
	_max = max;
	_lowtime = millis();
	_hightime = millis();
}

void HystCount::change(int val){
	_val += val;
	if (_val < 0)
		_val = 0;
	if (_val > _size)
		_val = _size;

	if (_active && _val < _min){
		_active = 0;
		_hightime = millis();
	}
	if (!_active && _val >= _max){
		_active= 1;
		_lowtime = millis();
	}
}

int HystCount::get_count(){
	return _val;
}

int HystCount::on(){
	return _active;
}

unsigned long HystCount::time_since_high(){
	if (_active)
		return 0;
	else 
		return millis()-_hightime;
}

unsigned long HystCount::time_since_low(){
	if (!_active)
		return 0;
	else
		return millis()-_lowtime;
}