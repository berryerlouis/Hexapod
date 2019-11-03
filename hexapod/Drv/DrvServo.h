/*
 * DrvServo.h
 *
 * Created: 28/06/2011 15:53:09
 *  Author: berryer
 */ 


#ifndef DRV_SERVO_H_
#define DRV_SERVO_H_

#include "../Tools/tools_typedefs.h"
////////////////////////////////////////////PUBLIC DEFINES///////////////////////////////////////////


#define SERVO_ANGLE_MID			90U		//mid position always 1500�s
#define SERVO_ANGLE_MIN			0U
#define SERVO_ANGLE_MAX			180U

#define PULSE_WIDTH_MIN			500U	//�s
#define PULSE_WIDTH_MID			1400U	//�s
#define PULSE_WIDTH_MAX			2300U	//�s

#define PERIOD_SERVO_MAX		15000U  //�s
#define NB_SERVOS				18U

#define MIN_SERVO_SPEED			0U
#define MAX_SERVO_SPEED			10U

#define SERVO_DEG_PER_SEC		50U

typedef enum
{
	E_SERVO_EASE_LINEAR, //no easing, no acceleration
	E_SERVO_EASE_SINUS_IN, //accelerating from zero velocity
	E_SERVO_EASE_SINUS_OUT, //decelerating to zero velocity
	E_SERVO_EASE_SINUS_IN_OUT //accelerating until halfway, then decelerating
}E_SERVO_EASES;

typedef void (*DrvServoCallback) (Int8U index);

/////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef struct
{
	Int8U id;
	Boolean enable;
	Int16S currentPosition;
	Int16S startPosition;
	Int16S targetPosition;
	Int32U startTime;
	Int16U movingTime;
	DrvServoCallback callback;
	Int16S min;
	Int16S mid;
	Int16S max;
	E_SERVO_EASES ease;
}SServo;


/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
// Init of Drv Servo 
Boolean DrvServoInit( void ) ;

// move the selected servo to the desired position [-900,900] and during the desired time
Boolean DrvServoSetTarget( Int8U index, Int16S angle , Int16U time ) ;
// set the callback to the selected servo
Boolean DrvServoSetCallback( Int8U index, DrvServoCallback callback ) ;
// get the target of the selected servo
Int16S DrvServoGetTarget( Int8U index ) ;
// activate or not the desired servo
Boolean DrvServoActivate( Int8U index, Boolean enable) ;
//check if servo reaches target
Boolean DrvServoCheckPosition( Int8U index ) ;
//compute and set the next angle
void DrvServoUpdate ( void ) ;

// get the servo structure
SServo* DrvServoGetStruture( Int8U index ) ;


#endif /* DRV_SERVO_H_ */