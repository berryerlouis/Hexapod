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


#define SERVO_ANGLE_MID			900U		//mid position always 1500�s
#define SERVO_ANGLE_MIN			0U
#define SERVO_ANGLE_MAX			1800U

#define PULSE_WIDTH_MIN			500U	//�s
#define PULSE_WIDTH_MID			1400U	//�s
#define PULSE_WIDTH_MAX			2300U	//�s

#define PERIOD_SERVO_MAX		15000U  //�s
#define NB_SERVOS				19U

/////////////////////////////////////////PUBLIC ENUM/////////////////////////////////////////


/////////////////////////////////////////PUBLIC CALLBACK/////////////////////////////////////////
//callback sent when the servo reach its position
typedef void (*DrvServoCallback) (Int8U index);

/////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
//servo structure representing all parameters of one servo
typedef struct
{
	Int8U id;
	Boolean enable;
	Int16S currentPosition;
	Int16S min;
	Int16S mid;
	Int16S max;
	Int16S offset;
}SServo;


/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
// Initialization of Servo
Boolean DrvServoInit( void ) ;

// move the selected servo to the desired position [-900,900] and during the desired time
Boolean DrvServoSetTarget( Int8U index, Int16S angle , Int16U time ) ;
// set the callback to the selected servo
Int16S DrvServoGetTarget( Int8U index ) ;
// activate or not the desired servo
Boolean DrvServoActivate( Int8U index, Boolean enable) ;
//compute and set the next angle
void DrvServoUpdate ( void ) ;

// get the servo structure
SServo* DrvServoGetStruture( Int8U index ) ;


#endif /* DRV_SERVO_H_ */