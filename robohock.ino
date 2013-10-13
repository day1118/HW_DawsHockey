/*
  Blink
 Turns on an LED on for one second, then off for one second, repeatedly.

 This example code is in the public domain.
 */

#include "config.h"
#include "statehelp.h"
#include "globals.h"
#include <Servo.h>
#define RAYTDICKEHAD

Servo servoF;  // create servo object to control a servo
Servo servoB;
Servo servoK;

#define LS 120
#define RS 164
#define TT 40
#define MAS 6

#define STICKY
#define STICKYDURATION 100

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led1 = COLOUR_GREEN_RED_PIN;
int led2 = COLOUR_RED_IR_PIN;

int IR_LED_13 = IR_LED_13_PIN;
int IR_LED_24 = IR_LED_24_PIN;

int delayTime = 4;

int BI = 0;
int BR = 0;

int GG = 0;
int GR = 0;

int IR1 = 0;
int IR2 = 0;
int IR3 = 0;
int IR4 = 0;

int IRD1 = 0;
int IRD2 = 0;
int IRD3 = 0;
int IRD4 = 0;

int IRN1 = 0;
int IRN2 = 0;
int IRN3 = 0;
int IRN4 = 0;

int counter = 0;

int Boff = 0;
int Bred = 0;
int Bgreen = 0;

int ss;

int DEBUG_PIN_1 = 40;


int difs[MAS];
int difs_loc = 0;

int froavg[MAS];
int froavg_loc = 0;

int ballRed = 0;
int ballBlue = 0;
int matRead = 0;

int state = 1;
int statetime;
int spin = 1;

unsigned long reverseT1 = 0;
unsigned long reverseT2 = 0;
unsigned long reverseT3 = 0;
int reverseStart;

unsigned long kickT1 = 0;
unsigned long kickT2 = 0;
unsigned long kickT3 = 0;
int kickStart = 1;

int driveStart = 1;
int driveDBall = 1;

int driveBallFound = 0;
int driveBallDisposal = 0;
int driveBallKeep = 0;
unsigned long driveT1 = 0;


int driveSticky;
unsigned long driveStickyT1;
int driveStickyDuration = 700;
float driveStickyPrevious;



int goalfindStart = 1;
unsigned long goalfindT1 = 1;
unsigned long goalfindT2 = 1;
int matFound = 0;
unsigned long P = 0;
int ballDropped = 1;

int quickspinStart = 1;
unsigned long quickspinT1;
float quickspinDir;

int overwatchRoundCount = 0;
unsigned long overwatchRoundStartTime = 0;
int overwatchState = 0;

int overrideEnable = 0;
int overrideType = 0;
unsigned long overrideT1;

int avgx =0;
// the setup routine runs once when you press reset:
void setup() {
    // initialize the digital pin as an output.
    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);
    pinMode(IR_LED_13_PIN, OUTPUT);
    pinMode(IR_LED_24_PIN, OUTPUT);
    pinMode(DEBUG_PIN_1, OUTPUT);
    pinMode(CAMERA_SI_PIN, OUTPUT);
    pinMode(CAMERA_CL_PIN, OUTPUT);

    pinMode(MOTOR_DIRECTION_L_1_PIN, OUTPUT);
    pinMode(MOTOR_DIRECTION_L_2_PIN, OUTPUT);
    pinMode(MOTOR_DIRECTION_R_1_PIN, OUTPUT);
    pinMode(MOTOR_DIRECTION_R_2_PIN, OUTPUT);
    pinMode(MOTOR_ENABLE_L_PIN, OUTPUT);
    pinMode(MOTOR_ENABLE_R_PIN, OUTPUT);

    servoF.attach(SERVO_FRONT_PIN);  // attaches the servo on pin 9 to the servo object
    servoB.attach(SERVO_BACK_PIN);
    servoK.attach(SERVO_KICK_PIN);

    Serial.begin(9600);      // open the serial port at 9600 bps:

    DEBUG_PRINT("String worked");

    analogWrite(MOTOR_ENABLE_L_PIN, 60);
    analogWrite(MOTOR_ENABLE_R_PIN, 60);
    digitalWrite(MOTOR_DIRECTION_L_1_PIN, HIGH);
    digitalWrite(MOTOR_DIRECTION_L_2_PIN, LOW);
    digitalWrite(MOTOR_DIRECTION_R_1_PIN, LOW);
    digitalWrite(MOTOR_DIRECTION_R_2_PIN, HIGH);

    ss =1;
    overwatchRoundStartTime = millis();
}

void newRound() {
    overwatchRoundCount += 1;
    unsigned long roundEndTime = millis();
    driveStickyDuration = 0;
    if (roundEndTime - overwatchRoundStartTime > 60 * 1000)
    {
        driveStickyDuration = 2000;
    }
    if (overwatchRoundCount % 2 == 1) {
        driveStickyDuration = 7000;
    }

    if (overwatchRoundCount % 2 == 1) {
        changeState(5);
    } else {
        changeState(1);
    }
    overwatchRoundStartTime = millis();
}

void cameraSignal()
{
    digitalWrite(CAMERA_SI_PIN, HIGH);
    delayMicroseconds(delayTime);               // wait for a second
    digitalWrite(CAMERA_CL_PIN, HIGH);
    delayMicroseconds(delayTime);               // wait for a second
    digitalWrite(CAMERA_SI_PIN, LOW);
    delayMicroseconds(delayTime);
}

void cameraRead()
{
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
            //center
            spin = 0;
        else if(center < 64)
            //center
            spin = 1;
        //right
    }

    PLOT_PRINT('\n');
    PLOT("AVGX", center);
    PLOT("SPIN", spin);
}

void drive(float dir) {
    int l,r;
    int dirl, dirr;
    dirl = 1;
    dirr = 1;
    if (dir >= 0) {
        l = LS + (dir * LS);
        r = RS - (dir * RS * 1.8);
        if (r < 0) {
            dirr = 0;
            r = r * -1;
        }
    }

    if (dir < 0) {
        l = LS + (dir * LS * 1.8);
        r = RS - (dir * RS);
        if (l < 0) {
            dirl = 0;
            l = l * -1;
        }
    }
    analogWrite(MOTOR_ENABLE_L_PIN, l);
    analogWrite(MOTOR_ENABLE_R_PIN, r);
    digitalWrite(MOTOR_DIRECTION_L_1_PIN, !dirr);
    digitalWrite(MOTOR_DIRECTION_L_2_PIN, dirr);
    digitalWrite(MOTOR_DIRECTION_R_1_PIN, !dirl);
    digitalWrite(MOTOR_DIRECTION_R_2_PIN, dirl);

}

void drivel() {
    analogWrite(MOTOR_ENABLE_L_PIN, 60);
    analogWrite(MOTOR_ENABLE_R_PIN, 60);
    digitalWrite(MOTOR_DIRECTION_L_1_PIN, 1);
    digitalWrite(MOTOR_DIRECTION_L_2_PIN, 0);
    digitalWrite(MOTOR_DIRECTION_R_1_PIN, 0);
    digitalWrite(MOTOR_DIRECTION_R_2_PIN, 1);

}

void driveb(float dir) {
    int l,r;
    int dirl, dirr;
    dirl = 1;
    dirr = 1;
    if (dir >= 0) {
        l = LS + (dir * LS);
        r = RS - (dir * RS * 1.8);
        if (r < 0) {
            dirr = 0;
            r = r * -1;
        }
    }

    if (dir < 0) {
        l = LS + (dir * LS * 1.8);
        r = RS - (dir * RS);
        if (l < 0) {
            dirl = 0;
            l = l * -1;
        }
    }
    analogWrite(MOTOR_ENABLE_L_PIN, l);
    analogWrite(MOTOR_ENABLE_R_PIN, r);
    digitalWrite(MOTOR_DIRECTION_L_1_PIN, dirr);
    digitalWrite(MOTOR_DIRECTION_L_2_PIN, !dirr);
    digitalWrite(MOTOR_DIRECTION_R_1_PIN, dirl);
    digitalWrite(MOTOR_DIRECTION_R_2_PIN, !dirl);

    PLOT("DIR", dir);
}

void brake() {
    digitalWrite(MOTOR_DIRECTION_L_1_PIN, 0);
    digitalWrite(MOTOR_DIRECTION_L_2_PIN, 0);
    digitalWrite(MOTOR_DIRECTION_R_1_PIN, 0);
    digitalWrite(MOTOR_DIRECTION_R_2_PIN, 0);

}

void state_quickspin() {
    if (quickspinStart) {
        quickspinT1 = millis() + random(300,700);
        quickspinDir = (float) (random(0,1) * 2 - 1);
        quickspinStart = 0;
    }
    if (millis() < quickspinT1) {
        drive(quickspinDir);
    } else {
        changeState(1);
    }

}

void state_goalfind() {
    if (goalfindStart) {
        matFound = 0;
        goalfindStart = 0;
        ballDropped = 0;
    }
    float dir_left = 0;
    float dir_right = 0;

    if (IRD3 > TT) {
        dir_left = 0.4 + 0.6 * (float) (IRD3 - TT) / (1000-TT);
    }
    else if (IRD4 > TT) {
        dir_right = -1.f * (0.4 + 0.6 * (float) (IRD4 - TT) / (1000-TT));
    }

    if (dir_left > 0.5 && dir_right > 0.5) {
        //TODO:: handle this case collision avoidance
        driveb(dir_left);
    } else {
        if (dir_left > dir_right)
            driveb(dir_left);
        else
            driveb(dir_right * -1);
    }


    if (ballRed < 4 && !ballDropped) {
        ballDropped = 1;
        goalfindT2 = millis() + 4000;
    }

    if (ballRed > 6 && ballDropped) {
        ballDropped = 0;
    }

    if (ballDropped && millis() > goalfindT2) {
        //state = 1;
        //driveStart = 1;
    }

    if (matRead > 8 && !matFound) {
        matFound = 1;
        goalfindT1 = millis() + 850;
    }
    if (matFound && millis() > goalfindT1) {
        changeState(3);
    }

}


void state_drive() {
    if (driveStart) {
        driveStart = 0;
        driveDBall = 1;

        driveBallKeep = 0;
        driveBallDisposal = 0;
        driveBallFound = 0;
    }
    float dir_left = 0;
    float dir_right = 0;
    float dir;
    if (IRD1 > TT) {
        dir_left = 0.4 + 0.6 * (float) (IRD1 - TT) / (1000-TT);
    }
    if (IRD2 > TT) {
        dir_right = 0.4 + 0.6 * (float) (IRD2 - TT) / (1000-TT);
    }


    if (dir_left > dir_right)
        dir = dir_left;
    else
        dir = dir_right * -1;





    if (dir_left >= 0.4 || dir_right >= 0.4) {
        driveSticky = 1;
        driveStickyPrevious = (dir_left > dir_right) ? dir_left : -1 * dir_right;
        driveStickyT1 = millis() + driveStickyDuration;
    }
    if (millis() > driveStickyT1) {
        driveSticky = 0;
    }

    if (dir == 0) {
        if (driveSticky) {
            dir = driveStickyPrevious;
        }
    }
    drive(dir);

    //if ball found flag that you have found
    if (ballRed > 4 || ballBlue > 4) {
        driveBallFound = 1;
    } else {
        driveBallFound = 0;
    }

    //set timer if ball is red to account for servo delay
    if (ballRed > 6 && !driveBallKeep) {
        driveBallKeep = 1;
        driveT1 = millis() + 300;
    }

    //hyseterisis for balldisposal
    if (ballBlue > 6 && !driveBallDisposal) {
        driveBallDisposal = 1;
    }
    if (ballBlue < 4 && driveBallDisposal) {
        driveBallDisposal = 0;
    }

    //if for any reason drop the gate
    if (driveBallFound || driveBallKeep || driveBallDisposal) {
        servoF.write(SERVO_FRONT_DOWN);
    } else {
        servoF.write(SERVO_FRONT_UP);
    }

    //if time is up after red ball changeState
    if (driveBallKeep && millis() >driveT1) {
        changeState(2);
    }

    //if ballneeds to be dsiposed lift front gate
    if (driveBallDisposal) {
        servoB.write(SERVO_BACK_UP);
    } else {
        servoB.write(SERVO_BACK_DOWN);
    }




}

void state_rotate() {
    if (reverseStart) {
        spin = 1;
        reverseT1 = millis() + 1000;
        reverseStart = 0;
    }

    if (millis() < reverseT1) {
        brake();
        return;
    }

    if (spin) {
        drive(-1);
        reverseT2 = millis() + 1800;
        reverseT3 = reverseT2 + 600;

        cameraRead();
    }
    else {
        unsigned long time = millis();
        if (time < reverseT2) {
            drive(0);
        } else if (time < reverseT3) {
            brake();
        } else {
            changeState(4);
        }
    }

}

void state_kick() {
    if (kickStart) {
        kickT1 = millis();
        kickT2 = kickT1 + 500;
        kickT3 = kickT2 + 300;
        kickStart = 0;
    }
    if (millis() > kickT1) {
        servoB.write(SERVO_FRONT_UP);
        servoF.write(SERVO_BACK_UP);
    }
    if (millis() > kickT2) {
        servoK.write(SERVO_KICK_DOWN);
    }
    if (millis() > kickT3) {
        servoB.write(SERVO_FRONT_DOWN);
        servoF.write(SERVO_FRONT_DOWN);
        servoK.write(SERVO_KICK_UP);

        newRound();
    }
}

void readIR() {
    digitalWrite(IR_LED_13, HIGH);   // turn the LED on
    delay(delayTime);
    IR1 = analogRead(IR_1_ANALOG_IN_PIN);
    IR3 = analogRead(IR_3_ANALOG_IN_PIN);
    digitalWrite(IR_LED_13, LOW);   // turn the LED on



    digitalWrite(IR_LED_24, HIGH);   // turn the LED on
    delay(delayTime);
    IR2 = analogRead(IR_2_ANALOG_IN_PIN);
    IR4 = analogRead(IR_4_ANALOG_IN_PIN);
    digitalWrite(IR_LED_24, LOW);   // turn the LED on

    delay(delayTime);

    IRN1 = analogRead(IR_1_ANALOG_IN_PIN);
    IRN2 = analogRead(IR_2_ANALOG_IN_PIN);
    IRN3 = analogRead(IR_3_ANALOG_IN_PIN);
    IRN4 = analogRead(IR_4_ANALOG_IN_PIN);

    IRD1 =  IR1 - IRN1;
    IRD2 =  IR2 - IRN2;
    IRD3 =  IR3 - IRN3;
    IRD4 =  IR4 - IRN4;
}

void readBall() {
    Boff = analogRead(BALL_ANALOG_IN_PIN);
    delay(delayTime);

    digitalWrite(COLOUR_RED_IR_PIN, HIGH);

    delay(delayTime);
    Bred = analogRead(BALL_ANALOG_IN_PIN);


    digitalWrite(COLOUR_RED_IR_PIN, LOW);

    digitalWrite(COLOUR_GREEN_RED_PIN, HIGH);

    delay(delayTime);
    Bgreen = analogRead(BALL_ANALOG_IN_PIN);
    digitalWrite(COLOUR_GREEN_RED_PIN, LOW);


}

void readGround() {

    digitalWrite(led2, LOW);   // turn the LED on (HIGH is the voltage level)
    digitalWrite(led1, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(delayTime);               // wait for a second
    BI = analogRead(BALL_ANALOG_IN_PIN);
    GR = analogRead(GROUND_ANALOG_IN_PIN);

    digitalWrite(led1, LOW);    // turn the LED off by making the voltage LOW
    digitalWrite(led2, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(delayTime);               // wait for a second
    BR = analogRead(BALL_ANALOG_IN_PIN);
    GG = analogRead(GROUND_ANALOG_IN_PIN);
    digitalWrite(led1, LOW);    // turn the LED off by making the voltage LOW
    digitalWrite(led2, LOW);   // turn the LED on (HIGH is the voltage level)
    PLOT("BI", BI);
    PLOT("GR", GR);
    PLOT("BR", BR);
    PLOT("GG", GG);
    PLOT("GD", GG-GR);
}

// the loop routine runs over and over again forever:
void loop() {

    /*    digitalWrite(led2, LOW);   // turn the LED on (HIGH is the voltage level)
     digitalWrite(led1, HIGH);   // turn the LED on (HIGH is the voltage level)
     delay(delayTime);               // wait for a second
     BI = analogRead(BALL_ANALOG_IN_PIN);
     GR = analogRead(GROUND_ANALOG_IN_PIN);

     digitalWrite(led1, LOW);    // turn the LED off by making the voltage LOW
     digitalWrite(led2, HIGH);   // turn the LED on (HIGH is the voltage level)
     delay(delayTime);               // wait for a second
     BR = analogRead(BALL_ANALOG_IN_PIN);
     GG = analogRead(GROUND_ANALOG_IN_PIN);
     digitalWrite(led1, LOW);    // turn the LED off by making the voltage LOW
     digitalWrite(led2, LOW);   // turn the LED on (HIGH is the voltage level)
     */
    readIR();


    difs[difs_loc] = IRD1-IRD2;
    froavg[froavg_loc] = (IRD1 + IRD2) / 2;

    difs_loc = (difs_loc + 1) % MAS;
    froavg_loc = (froavg_loc + 1) % MAS;


    readBall();
    if (Bgreen > 300) {
        if (Bred > 220) {
            ballRed += 2;
        } else {
            ballBlue += 2;
        }
    }
    ballRed -= 1;
    ballBlue -= 1;

    ballRed = (ballRed < 0) ? 0 : ballRed;
    ballBlue= (ballBlue < 0) ? 0 :ballBlue;
    ballRed = (ballRed > 10) ? 10 : ballRed;
    ballBlue = (ballBlue > 10) ? 10: ballBlue;

    readGround();
    if (GG-GR > 20) {
        matRead += 1;
    } else
        matRead -= 1;

    matRead = (matRead > 10) ? 10 : matRead;
    matRead = (matRead < 0 ) ? 0  : matRead;





    PLOT("BALLBLUE", ballBlue);
    PLOT("BALLRED", ballRed);

    PLOT("BOFF", Boff);
    PLOT("BRED", Bred);
    PLOT("BGREEN", Bgreen);

    switch (state) {
    case 1:
        state_drive();
        break;
    case 2:
        state_goalfind();
        break;
    case 3:
        state_rotate();
        break;
    case 4:
        state_kick();
        break;
    case 5:
        state_quickspin();
        break;
    }
    PLOT("STATE", state);
    PLOT("MATREAD",matRead);
    PLOT("BALLRED", ballRed);
    PLOT("BALLBLUE", ballBlue);

    PLOT("IR1", IR1);
    PLOT("IR2", IR2);
    PLOT("IR3", IR3);
    PLOT("IR4", IR4);
    PLOT("IRN1", IRN1);
    PLOT("IRD1", IRD1);
    PLOT("IRN2", IRN2);
    PLOT("IRD2", IRD2);
    PLOT("IRN3", IRN3);
    PLOT("IRD3", IRD3);
    PLOT("IRN4", IRN4);
    PLOT("IRD4", IRD4);
    /*digitalWrite(MOTOR_ENABLE_R_PIN, HIGH);
     digitalWrite(MOTOR_DIRECTION_R_1_PIN, HIGH);
     digitalWrite(MOTOR_DIRECTION_R_2_PIN, LOW);*/

    /*servoK.write(SERVO_KICK_UP);
     delay(1000);
     servoF.write(SERVO_FRONT_DOWN);
     servoB.write(SERVO_BACK_DOWN);
     delay(1000);
     servoF.write(SERVO_FRONT_UP);
     servoB.write(SERVO_BACK_UP);
     delay(1000);
     servoK.write(SERVO_KICK_DOWN);
     delay(1000);*/
}

