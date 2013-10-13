#include "config.h"

#include "motors.h"
#include "ir.h"
#include "hysteresiscounter.h"

#include <Servo.h>
#include <NewPing.h>

#define LS 210
#define RS 210


NewPing USFront(FRONT_ULTRASONIC_SENSOR_TRIG,
	FRONT_ULTRASONIC_SENSOR_ECHO,
	MAX_ULTRASONIC_DISTANCE_CM);

//IRDEFINTION
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

//motor definition

Motor motorl(MOTOR_L_A_PIN,
	MOTOR_L_B_PIN,
	MOTOR_L_ENABLE_PIN);

Motor motorr(MOTOR_R_A_PIN,
	MOTOR_R_B_PIN,
	MOTOR_R_ENABLE_PIN);

Motor motorb(MOTOR_B_A_PIN,
	MOTOR_B_B_PIN,
	MOTOR_B_ENABLE_PIN);

PT2Val	matL(GREEN_MAT_LEFT_RED_LED_PIN,
	GREEN_MAT_LEFT_GREEN_LED_PIN,
	GREEN_MAT_LEFT_PHOTOTRANSISTOR_PIN);

PT2Val	matR(GREEN_MAT_RIGHT_RED_LED_PIN,
	GREEN_MAT_RIGHT_GREEN_LED_PIN,
	GREEN_MAT_RIGHT_PHOTOTRANSISTOR_PIN);

PT2Val ball(BALL_COLOUR_RED_LED_PIN,
	BALL_COLOUR_IR_LED_PIN,
	BALL_COLOUR_PHOTOTRANSISTOR_PIN);

Servo servoF;
Servo servoB;
Servo servoK;

HystCount ballRed(10, 4, 7);
HystCount ballBlue(10, 4, 7);
HystCount greenMatL(10, 4, 7);
HystCount greenMatR(10, 4, 7);



int state = STATE_BALLFIND;
int spin = 2;

int overwatchRoundCount = 0;
unsigned long overwatchRoundStartTime;
int overwatchState;

int overrideEnable = 0;
int overrideSubState = 0;
int overrideType = 0;
int overrideFreakoutLevel = 0;

struct OverrideState{
	int duration;
	int Otype;
	int Osubdata;
};

struct OverrideState Ostate0;
struct OverrideState Ostate1;
struct OverrideState Ostate2;

struct OverrideState* oStates[3];

int oState;


unsigned long overrideTLast;
unsigned long overrideT1;


int driveStart;

int goalfindStart;

int orientateStart;
unsigned long orientateT1;
unsigned long orientateT2;

int kickStart;
unsigned long kickT1;
unsigned long kickT2;
unsigned long kickT3;
unsigned long kickT4;
unsigned long kickT5;
unsigned long kickT6;
unsigned long kickT7;

int quickreverseStart = 1;
unsigned long quickreverseT1;

unsigned long sampleCount = 0;

int stallIRFL[STALL_SIZE];
int stallIRFR[STALL_SIZE];
int stallIRBL[STALL_SIZE];
int stallIRBR[STALL_SIZE];

int stallPos = 0;
int stallFilled =0;


void cameraSignal(){
  digitalWrite(CAMERA_SI_PIN, HIGH);
  delayMicroseconds(4);               // wait for a second
  digitalWrite(CAMERA_CL_PIN, HIGH);
  delayMicroseconds(4);               // wait for a second
  digitalWrite(CAMERA_SI_PIN, LOW);
  delayMicroseconds(4);
  digitalWrite(CAMERA_CL_PIN, LOW);
}

int cameraRead(){
  cameraSignal();

  for (int i =0; i < 128; i++) {
    digitalWrite(CAMERA_CL_PIN, HIGH);
    digitalWrite(CAMERA_CL_PIN, LOW);
  }

  delayMicroseconds(100);

  cameraSignal();

  PLOT_PRINT("CAMERA");

  float avgx = 0;
  int startPoint = 0;
  int conseq = 0;
  int bestStart = 0;
  int bestWidth = 0;
  
  for(int i = 0; i < 128; i ++) {
    digitalWrite(CAMERA_CL_PIN, HIGH);
    int cameraSensorValue = analogRead(CAMERA_ANALOG_IN_PIN );
    digitalWrite(CAMERA_CL_PIN, LOW);
    PLOT_PRINT(':');
    PLOT_PRINT(cameraSensorValue);
    if(cameraSensorValue > 300)
    {
      if(conseq == 0)
      {
        startPoint = i;
      }
      conseq ++;
      if(conseq > bestWidth)
      {
        bestStart = startPoint;
        bestWidth = conseq;
      }
    }
    else
    {
      conseq = 0;
    }
  }

  int center = bestStart + (bestWidth/2);
  if(bestWidth > 10)
  {
    if(center > 40 && center < 88)
      spin = 0;
    else if(center < 64)
      spin = 1;
  }

  PLOT_PRINT('\n');
  PLOT("AVGX", center);
  PLOT("SPIN", spin);

  return center;
}

void read_ir(){
	irfl.read();
	irfr.read();
	irbl.read();
	irbr.read();
	matL.read();
	matR.read();
	ball.read();
}

void drivef(int dir){
	if (dir > 100)
		dir = 100;
	if (dir < -100)
		dir = -100;

	int motorLdir = 1;
	int motorRdir = 1;

	int motorLspeed = LS;
	int motorRspeed = RS;

	if (dir > 0)
		motorRspeed += - (dir * RS / 100) * 2;
	if (dir < 0)
		motorLspeed +=  (dir * LS / 100) * 2;
	if (motorRspeed < 0)
		motorRdir = 0;
	if (motorLspeed < 0)
		motorLdir = 0;

	if (motorLdir){
		motorl.drivef(motorLspeed);
	} else {
		motorl.driveb(motorLspeed * -1);
	}

	if (motorRdir){
		motorr.drivef(motorRspeed);
	} else {
		motorr.driveb(motorRspeed * -1);
	}

	PLOT("DIR", dir);
	PLOT("FOR", 1);
	PLOT("MLS", motorLspeed);
	PLOT("MRS", motorRspeed);
}

void spinslow(int dir){

		motorl.drivef(6*LS/8);
		motorr.driveb(6*RS/8);
}

void driveb(int dir){
	if (dir > 100){
		dir = 100;
	}
	if (dir < -100)
		dir = -100;


	int motorLdir = 1;
	int motorRdir = 1;

	int motorLspeed = LS;
	int motorRspeed = RS;

	if (dir > 0)
		motorRspeed += -2 * (dir * RS / 100);
	if (dir < 0)
		motorLspeed +=  2 * (dir * LS / 100);

	if (motorRspeed < 0)
		motorRdir = 0;
	if (motorLspeed < 0)
		motorLdir = 0;

	if (motorLdir){
		motorl.driveb(motorLspeed);
	} else {
		motorl.drivef(motorLspeed * -1);
	}

	if (motorRdir){
		motorr.driveb(motorRspeed);
	} else {
		motorr.drivef(motorRspeed * -1);
	}

	PLOT("DIR", dir);
	PLOT("FOR", -1);
}

void brake(){
	motorl.brake();
	motorr.brake();
}


void sensing(){
	if (ball.dif2() > 800 && ball.dif1() > 800)
		ballRed.change(1);
	else
		ballRed.change(-1);

	if (ball.dif2() > 800 && ball.dif1() < 400)
		ballBlue.change(1);
	else
		ballBlue.change(-1);
	//TODO UNCOMMENT FOR GREEN MAT
	/*
	if (matL.dif1() < 600 && matL.dif2() < 600)
		greenMatL.change(1);
	else 
		greenMatL.change(-1);

	if (matR.dif1() < 700 && matR.dif2() < 900)
		greenMatR.change(1);
	else 
		greenMatR.change(-1);
		*/

	if (matL.dif2() < 480)
		greenMatL.change(1);
	else
		greenMatL.change(-1);

	if (matR.dif1() < 800 && matR.dif2())
		greenMatR.change(1);
	else
		greenMatR.change(-1);	

}

void touch_sensing(){
	int tsl = digitalRead(FRONT_LEFT_TOUCH_SENSOR);
	int tsr = digitalRead(FRONT_RIGHT_TOUCH_SENSOR);
	int tbl = digitalRead(BACK_LEFT_TOUCH_SENSOR);
	int tbr = digitalRead(BACK_RIGHT_TOUCH_SENSOR);
	if (!tsl || !tsr){
		overrideEnable = 1;
		oState = 1;

		if (!tsr){
			overrideEnable = 1;

			oStates[0]->Otype = OVERR_REVERSE;
			oStates[0]->Osubdata = 100;
			oStates[0]->duration = 250;

			oStates[1]->Otype = OVERR_REVERSE;
			oStates[1]->Osubdata = 0;
			oStates[1]->duration = 800;
		
			oState = 1;

		}

		if (!tsl){
			overrideEnable = 1;

			oStates[0]->Otype = OVERR_REVERSE;
			oStates[0]->Osubdata = -100;
			oStates[0]->duration = 250;

			oStates[1]->Otype = OVERR_REVERSE;
			oStates[1]->Osubdata = 0;
			oStates[1]->duration = 800;
		
			oState = 1;

		}

		overrideT1 = millis() + oStates[oState]->duration;
	} 
	if (!tbl || !tbr){
		overrideEnable = 1;
		overrideSubState = 1;
		overrideType = OVERR_FORWARD;
		overrideT1 = millis() + 400;
	}
}

void changeState(int newState){
	state = newState;
	switch (newState){
		case STATE_BALLFIND:
			driveStart = 1;
			break;
		case STATE_GOALFIND:
			goalfindStart = 1;
			break;
		case STATE_ORIENTATE:
			orientateStart = 1;
			break;
		case STATE_KICK:
			kickStart = 1;
			break;
		case STATE_QUICKREVERSE:
			quickreverseStart = 1;
			break;
	}
}

void newRound(){
	overwatchRoundCount += 1;
	changeState(STATE_BALLFIND);
	overwatchRoundStartTime = millis();	
}

void overrideHandler(){

	if (overrideEnable && millis() < overrideT1){
		struct OverrideState* cState = oStates[oState]; 
		switch (cState->Otype){
			case OVERR_REVERSE:
				driveb(cState->Osubdata);
				break;
			case OVERR_FORWARD:
				drivef(cState->Osubdata);
				break;
			case OVERR_PAUSE:
				break;
		}	
	}


	if (overrideEnable && millis() > overrideT1){
		oState -= 1;
		if (oState < 0){
			overrideEnable = 0;
		} else {
			overrideT1 += oStates[oState]->duration; 
		}
	}
}

void findMinMax(int* valArray, int* min, int* max){
	int tmin = 1025;
	int tmax = -1;

	for (int i =0; i < STALL_SIZE; i++){
		if (valArray[i] > tmax)
			tmax = valArray[i];
		if (valArray[i] < tmin)
			tmin = valArray[i];
	}

	(*min) = tmin;
	(*max) = tmax;
}

void stall_detection(){

	stallIRFL[stallPos] = irfl.getf();
	stallIRFR[stallPos] = irfr.getf();
	stallIRBL[stallPos] = irbl.getf();
	stallIRBR[stallPos] = irbr.getf();
	stallPos += 1;


	if (stallPos >= STALL_SIZE){
		stallFilled = 1;
		stallPos = 0;
	}

	if (!(stallFilled && stallPos % 5 == 0))
		return;

	int minStallIRFL, maxStallIRFL;
	int minStallIRFR, maxStallIRFR;
	int minStallIRBL, maxStallIRBL;
	int minStallIRBR, maxStallIRBR;

	findMinMax(stallIRFL, &minStallIRFL, &maxStallIRFL);
	findMinMax(stallIRFR, &minStallIRFR, &maxStallIRFR);
	findMinMax(stallIRBL, &minStallIRBL, &maxStallIRBL);
	findMinMax(stallIRBR, &minStallIRBR, &maxStallIRBR);


	if (maxStallIRFL - minStallIRFL < 150 ){
		overrideEnable = 1;

		oStates[0]->Otype = OVERR_REVERSE;
		oStates[0]->Osubdata = 0;
		oStates[0]->duration = 500;
		
		oState = 0;

		overrideT1 = millis() + oStates[0]->duration;
	}

	if (maxStallIRFR - minStallIRFR < 150){

		overrideEnable = 1;

		oStates[0]->Otype = OVERR_REVERSE;
		oStates[0]->Osubdata = 0;
		oStates[0]->duration = 500;
		
		oState = 0;

		overrideT1 = millis() + oStates[0]->duration;
	}


	if (maxStallIRBL - minStallIRBL < 150){
		overrideEnable = 1;

		oStates[0]->Otype = OVERR_FORWARD;
		oStates[0]->Osubdata = 0;
		oStates[0]->duration = 500;
		
		oState = 0;

		overrideT1 = millis() + oStates[0]->duration;
	}

	if (maxStallIRBR - minStallIRBR < 150){
		overrideEnable = 1;

		oStates[0]->Otype = OVERR_FORWARD;
		oStates[0]->Osubdata = 0;
		oStates[0]->duration = 500;
		
		oState = 0;

		overrideT1 = millis() + oStates[0]->duration;
	}
}



void stateDrive(){
	if (driveStart){
		motorb.drivef(200);
		driveStart =0;
	}

	motorb.drivef(150);

	int flf = irfl.getf();
	int frf = irfr.getf();

	int fls = irfl.gets();
	int frs = irfr.gets();

	int bls = irbl.gets();
	int brs = irbr.gets();

	int dirL = 0;
	int dirR = 0;

	int dirLA = 0;
	int dirRA = 0;
	int dforward = 1;



	if (fls > 30){
		dirL = 1;
		dirLA = 100;
	}

	if (flf > 10){
		dirL = 1;
		dirLA = (flf-10) * 15;
	}
	if (frs > 30){
		dirR = 1;
		dirRA = 100;
	}

	if (frf > 10){
		dirR = 1;
		dirRA = (frf-10) * 15;
	}


/*	if (overrideEnable)
		overrideHandler();
	else{
		if (greenMatL.on() || greenMatR.on()){
			//TODO uncomment and test
			int center = cameraRead();
			if (center > 10 && center < 120){
				drivef(100);
			}
		}	else { */

	if (overrideEnable){
		overrideHandler();
	}else {
		if (dirL){
			drivef(dirLA);
		} else if (dirR){
			drivef(-1 * dirRA);
		} else {
			drivef(0);
		}/*
			if (dirL){
				drivef(dirLA);
			} else if (dirR){
				drivef(dirRA * -1);
			} else {
				drivef(0);
			}*/
	}


	if (ballRed.on() || ballBlue.on()){
		servoF.write(SERVO_FRONT_DOWN);
	} else {
		servoF.write(SERVO_FRONT_UP);
	}

	if (ballRed.on()){
		servoB.write(SERVO_BACK_UP);
	} else {
		servoB.write(SERVO_BACK_DOWN);
	}

	if (ballBlue.time_since_low() > 400){
		changeState(STATE_GOALFIND);
	}
}

void stateGoalfind(){
	if (goalfindStart){
		goalfindStart = 0;
	}	

	int blf = irbl.getf();
	int brf = irbr.getf();


	int bls = irbl.gets();
	int brs = irbr.gets();

	int dirL = 0;
	int dirLA = 0;

	int dirR = 0;
	int dirRA = 0;
	int dforward = 1;



	if (bls > 30){
		dirL = 1;
		dirLA = 100;
	}

	if (blf > 10){
		dirL = 1;
		dirLA = (blf-10) * 15;
	}
	if (brs > 30){
		dirR = 1;
		dirRA = 100;
	}

	if (brf > 10){
		dirR = 1;
		dirRA = (brf-10) * 15;
	}

	if (overrideEnable){
		overrideHandler();
	} else {
		if (dirLA){
			driveb(dirL);
		} else if (dirRA){
			driveb(dirR * -1);
		} else {
			driveb(0);
		}
	}

	#define GFIND_MAT_DELAY	60

	if (greenMatL.on() || greenMatR.on()){
		spin = 1; 
		cameraRead();
		if (!spin){
			changeState(STATE_QUICKREVERSE);
		}
	}

	if (greenMatL.on())
		if (greenMatL.time_since_low() > GFIND_MAT_DELAY)
			changeState(STATE_ORIENTATE);

	if (greenMatR.on())
		if (greenMatR.time_since_low() > GFIND_MAT_DELAY)
			changeState(STATE_ORIENTATE);	
}

void stateOrientate(){
	if (orientateStart){
		spin = 1;
		orientateStart = 0;
	}


	if (spin){
		spinslow(1);
		cameraRead();
		orientateT1 = millis() + 1000;
	} else {
		drivef(0);
		if (millis() > orientateT1){
			brake();
			int peak_pos = cameraRead();
			if (peak_pos < 40){
				drivef(100);
				delay(100);
				brake();
			}
			if (peak_pos > 90){
				drivef(-100);
				delay(100);
				brake();
			}
			changeState(STATE_KICK);
		}
	}
}

void stateKick(){
	if (kickStart){
		kickStart = 0;
		kickT1 = millis() + 300;
		kickT2 = kickT1 + 300;
		kickT3 = kickT2 + 300;
		kickT4 = kickT3 + 300;
		kickT5 = kickT4 + 300;
		kickT6 = kickT5 + 300;
	}
	unsigned long curtime = millis();

	if (curtime > kickT1 && curtime < kickT2){
		servoB.write(SERVO_BACK_UP);
	}
	if (curtime > kickT2 && curtime < kickT3){
		servoF.write(SERVO_FRONT_UP);
	}
	if (curtime > kickT3 && curtime < kickT4){
		servoK.write(SERVO_KICK_DOWN);
	}
	if (curtime > kickT4 && curtime < kickT5){
		servoK.write(SERVO_KICK_UP);
	}
	if (curtime > kickT5 && curtime < kickT6){
		servoB.write(SERVO_BACK_DOWN);
	}
	if (curtime > kickT6){
		newRound();	
	}
}

void stateQuickReverse(){
	if (quickreverseStart){
		quickreverseT1 = millis() + 400;
		quickreverseStart = 0;
	}
	if (millis() < quickreverseT1){
		drivef(0);
	} else {
		brake();
		changeState(STATE_KICK);
	}
}



void setup(){
	Serial.begin(115200);

    servoF.attach(SERVO_FRONT_PIN,
    	SERVO_FRONT_MIN_PWM,
    	SERVO_FRONT_MAX_PWM);
    servoB.attach(SERVO_BACK_PIN,
    	SERVO_BACK_MIN_PWM,
    	SERVO_BACK_MAX_PWM);
    servoK.attach(SERVO_KICK_PIN,
    	SERVO_KICK_MIN_PWM,
    	SERVO_KICK_MAX_PWM);

    pinMode(CAMERA_SI_PIN, OUTPUT);
    pinMode(CAMERA_CL_PIN, OUTPUT);
    pinMode(CAMERA_ANALOG_IN_PIN, INPUT);
	pinMode(FRONT_LEFT_TOUCH_SENSOR, INPUT);
	pinMode(FRONT_RIGHT_TOUCH_SENSOR, INPUT);
	pinMode(BACK_LEFT_TOUCH_SENSOR, INPUT);
	pinMode(BACK_RIGHT_TOUCH_SENSOR, INPUT);

	servoK.write(SERVO_KICK_UP);

	memset(&Ostate0, 0, sizeof(Ostate0));
	memset(&Ostate1, 0, sizeof(Ostate1));
	memset(&Ostate2, 0, sizeof(Ostate2));

	oStates[0] = &Ostate0;
	oStates[1] = &Ostate1;
	oStates[2] = &Ostate2;
}

void loop(){ 


	read_ir();

	sensing();

	touch_sensing();

	switch (state){
		case STATE_BALLFIND:
			stateDrive();
			break;
		case STATE_GOALFIND:
			stateGoalfind();
			break;
		case STATE_ORIENTATE:
			stateOrientate();
			break;
		case STATE_KICK:
			stateKick();
			break;
		case STATE_QUICKREVERSE:
			stateQuickReverse();
			break;
		}
	if (sampleCount % 5 == 0)
		//stall_detection();

	sampleCount += 1;

	PLOT("STATE",state);
	PLOT("SPIN", spin);



	PLOT("IRFLF", irfl.getf());
	PLOT("IRFLS", irfl.gets());
	PLOT("IRFRF", irfr.getf());
	PLOT("IRFRS", irfr.gets());
	PLOT("IRBLF", irbl.getf());
	PLOT("IRBLS", irbl.gets());
	PLOT("IRBRF", irbr.getf());
	PLOT("IRBRS", irbr.gets());

	PLOT("MATL1", matL.dif1());
	PLOT("MATL2", matL.dif2());
	PLOT("MATR1", matR.dif1());
	PLOT("MATR2", matR.dif2());


	PLOT("OVERR_ENABLE", overrideEnable);
	PLOT("OVERR_TYPE", oState);
	PLOT("OVERR_STYPE", overrideSubState);
	PLOT("OVERR_T1", overrideT1);

	PLOT("RED", ballRed.get_count());
	PLOT("BLUE", ballBlue.get_count());
 
 	delay(4);	

}