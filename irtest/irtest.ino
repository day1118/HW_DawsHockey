#include "ir.h"
#include "config.h"


IRDuo irfl(
	IRFL_IR_LED_PIN,
	IRFL_FRONT_PHOTOTRANSISTOR_PIN,
	IRFL_SIDE_PHOTOTRANSISTOR_PIN);
IRDuo irfr(
	IRFR_IR_LED_PIN,
	IRFR_FRONT_PHOTOTRANSISTOR_PIN,
	IRFR_SIDE_PHOTOTRANSISTOR_PIN);
IRDuo irbl(
	IRBL_IR_LED_PIN,
	IRBL_BACK_PHOTOTRANSISTOR_PIN,
	IRBL_SIDE_PHOTOTRANSISTOR_PIN);
IRDuo irbr(
	IRBR_IR_LED_PIN,
	IRBR_BACK_PHOTOTRANSISTOR_PIN,
	IRBR_SIDE_PHOTOTRANSISTOR_PIN);


void setup(){
	Serial.begin(9600);
}

void loop(){

	irfl.read();
	irfr.read();
	irbl.read();
	irbr.read();

	Serial.print(irfl.getf());
	Serial.print(irfl.gets());
	Serial.print(irfr.getf());
	Serial.print(irfr.gets());
	Serial.print(irbl.getf());
	Serial.print(irbl.gets());
	Serial.print(irbr.getf());
	Serial.print(irbr.gets());

}