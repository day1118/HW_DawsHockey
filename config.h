/*** Define debug level ***/
#define	DEBUG_PRINT_ON
#define	PLOT_PRINT_ON

#define LED_READ_DELAY_TIME                 50
/*** Define named positions ***/
#define SERVO_FRONT_UP                      0
#define SERVO_FRONT_DOWN                    140
#define SERVO_BACK_UP                       0
#define SERVO_BACK_DOWN                     140

#define SERVO_KICK_UP                       90
#define SERVO_KICK_DOWN                     10

/*** Define pinouts ***/
#define FRONT_ULTRASONIC_SENSOR_TRIG        31
#define FRONT_ULTRASONIC_SENSOR_ECHO        33

#define IRFL_IR_LED_PIN                     32
#define IRFL_SIDE_PHOTOTRANSISTOR_PIN       A10
#define IRFL_FRONT_PHOTOTRANSISTOR_PIN      A9

#define GREEN_MAT_LEFT_RED_LED_PIN          24
#define GREEN_MAT_LEFT_GREEN_LED_PIN        26
#define GREEN_MAT_LEFT_PHOTOTRANSISTOR_PIN  A12

#define BALL_COLOUR_RED_LED_PIN             28
#define BALL_COLOUR_IR_LED_PIN              30
#define BALL_COLOUR_PHOTOTRANSISTOR_PIN     A11

#define SERVO_KICK_PIN			    4
#define SERVO_FRONT_PIN			    2
#define SERVO_BACK_PIN			    3

#define IRBL_IR_LED_PIN                     25
#define IRBL_SIDE_PHOTOTRANSISTOR_PIN       A15
#define IRBL_BACK_PHOTOTRANSISTOR_PIN       A14

#define CAMERA_ANALOG_IN_PIN                A13
#define CAMERA_CLK_PIN                      27  // May be wrong
#define CAMERA_SI_PIN                       29  // May be wrong

#define FRONT_LEFT_TOUCH_SENSOR             22
#define BACK_LEFT_TOUCH_SENSOR              23
#define FRONT_RIGHT_TOUCH_SENSOR            42
#define BACK_RIGHT_TOUCH_SENSOR             43

#define IRFR_IR_LED_PIN                     46
#define IRFR_SIDE_PHOTOTRANSISTOR_PIN       A4
#define IRFR_FRONT_PHOTOTRANSISTOR_PIN      A5

#define BACK_ULTRASONIC_SENSOR_TRIG         48
#define BACK_ULTRASONIC_SENSOR_ECHO         44

#define GREEN_MAT_RIGHT_RED_LED_PIN         45
#define GREEN_MAT_RIGHT_GREEN_LED_PIN       49
#define GREEN_MAT_RIGHT_PHOTOTRANSISTOR_PIN A8

#define IRBR_IR_LED_PIN                     57
#define IRBR_SIDE_PHOTOTRANSISTOR_PIN       A6
#define IRBR_BACK_PHOTOTRANSISTOR_PIN       A7

#define MOTOR_L_A_PIN		            35
#define MOTOR_L_B_PIN		            34
#define MOTOR_L_ENABLE_PIN		    7

#define MOTOR_R_A_PIN		            37
#define MOTOR_R_B_PIN		            36
#define MOTOR_R_ENABLE_PIN		    6

#define MOTOR_B_A_PIN		            39
#define MOTOR_B_B_PIN		            38
#define MOTOR_B_ENABLE_PIN		    5

/*** Define print statements ***/
#ifdef DEBUG_PRINT_ON
  #define DEBUG_PRINT(x)        Serial.print (x)
  #define DEBUG_PRINTDEC(x)     Serial.print (x, DEC)
  #define DEBUG_PRINTLN(x)      Serial.println (x)
#else
  #define DEBUG_PRINT(x)        Serial.print ("n" + x)
  #define DEBUG_PRINTDEC(x)
  #define DEBUG_PRINTLN(x) 
#endif

#ifdef PLOT_PRINT_ON
  #define PLOT_PRINT(x)        Serial.print (x)
  #define PLOT_PRINTDEC(x)     Serial.print (x, DEC)
  #define PLOT_PRINTLN(x)      Serial.println (x)
  #define PLOT(x, y)           PLOT_PRINT(x); PLOT_PRINT('-'); PLOT_PRINTLN(y);
#else
  #define PLOT_PRINT(x)
  #define PLOT_PRINTDEC(x)
  #define PLOT_PRINTLN(x) 
#endif
