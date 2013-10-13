#include "config.h"

int spin;

void cameraSignal(){
  digitalWrite(CAMERA_SI_PIN, HIGH);
  delayMicroseconds(4);               // wait for a second
  digitalWrite(CAMERA_CL_PIN, HIGH);
  delayMicroseconds(4);               // wait for a second
  digitalWrite(CAMERA_SI_PIN, LOW);
  delayMicroseconds(4);
}

void cameraRead(){
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

void setup(){

    pinMode(CAMERA_SI_PIN, OUTPUT);
    pinMode(CAMERA_CL_PIN, OUTPUT);
    pinMode(CAMERA_ANALOG_IN_PIN, INPUT);

    Serial.begin(9600);
}

void loop(){
    cameraRead();
}