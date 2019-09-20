/*
* DrvServo.c
*
* Created: 28/06/2011 15:52:35
*  Author: berryer
*/
////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "../Conf/ConfHard.h"
#include "DrvServo.h"
#include "DrvTick.h"
#include "../Cmps/CmpPCA9685.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define SERVO_FREQ	60U

#define MIN_SERVO_DURATION			10000U
#define MAX_SERVO_DURATION			100U
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
static void DrvServoComputeAngle ( Int8U index );
////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
static SServo servos[NB_SERVOS];



/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
// Init of Drv Servo
Boolean DrvServoInit( void )
{
	//set servos to default value
	for (Int8U index = 0U; index < NB_SERVOS ; index++)
	{
		servos[ index ].id = index;
		servos[ index ].currentPosition = 0;
		servos[ index ].startPosition = 0;
		servos[ index ].targetPosition = 0;
		servos[ index ].movingTime = 0U;
		servos[ index ].startTime = 0U;
		servos[ index ].callback = NULL;
		servos[ index ].min = SERVO_ANGLE_MIN;
		servos[ index ].mid = SERVO_ANGLE_MID;
		servos[ index ].max = SERVO_ANGLE_MAX;
		servos[ index ].ease = E_SERVO_EASE_LINEAR;
	}
	
	//initialize the 2 components PCA9685
	CmpPCA9685Init(PCA9685_ADDRESS_0);
	CmpPCA9685Init(PCA9685_ADDRESS_1);
	//set default PWM frequency to 60Hz
	CmpPCA9685SetPWMFreq(PCA9685_ADDRESS_0, SERVO_FREQ);
	CmpPCA9685SetPWMFreq(PCA9685_ADDRESS_1, SERVO_FREQ);

	return TRUE;
}

// move the selected servo to the desired position [-900,900] and during the desired time
Boolean DrvServoSetTarget( Int8U index, Int16S angle, Int16U time)
{
	//check index
	if( index < NB_SERVOS )
	{
		//if between limits
		if(( angle >= servos[ index ].min ) && ( angle <= servos[ index ].max ))
		{
			servos[ index ].movingTime = time;
			//if not already set
			if(servos[ index ].targetPosition != angle)
			{
				servos[ index ].startPosition = servos[ index ].currentPosition;
				servos[ index ].targetPosition = angle;
				servos[ index ].startTime = DrvTickGetTimeMs();
				return TRUE;
			}
		}
	}
	return FALSE;
}

// set the callback to the selected servo
Boolean DrvServoSetCallback( Int8U index, DrvServoCallback callback )
{
	servos[ index ].callback = callback;
	return TRUE;
}

// get the target of the selected servo
Int16S DrvServoGetTarget( Int8U index )
{
	return servos[ index ].targetPosition;
}

//check if servo reaches target
Boolean DrvServoCheckPosition( Int8U index )
{
	if( servos[index].currentPosition != servos[index].targetPosition ) 
	{
		return FALSE;
	}
	return TRUE;
}

//set the position
void DrvServoUpdate ( void )
{
	//fill the pwm for each servos
	SPCA9685Pwm pwm[NB_SERVOS];
	for (Int8U index = 0U; index < NB_SERVOS ; index++)
	{
		DrvServoComputeAngle(index);
		Int16S angle = servos[index].currentPosition;
		
		//convert to 0� to 180,0�
		angle += 90;
		angle = SetRangeInt16(angle, SERVO_ANGLE_MIN, SERVO_ANGLE_MAX, 114, 522);
		pwm[index].on = 0U;
		pwm[index].off = angle;
	}
	//send to each component
	CmpPCA9685SetAllPWM(PCA9685_ADDRESS_0,pwm,NB_SERVOS/2U);
	CmpPCA9685SetAllPWM(PCA9685_ADDRESS_1,&pwm[NB_SERVOS/2U],NB_SERVOS/2U);	
}

// get the servo strucutre
SServo* DrvServoGetStruture( Int8U index )
{
	if( index < NB_SERVOS )
	{
		return &servos[ index ];
	}
	return NULL;
}

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

void DrvServoComputeAngle ( Int8U index )
{
	Int32U currentTime	= DrvTickGetTimeMs() - servos[ index ].startTime;
	Int16S startValue	= servos[ index ].startPosition;
	Int16S changeValue	= servos[ index ].targetPosition - startValue;
	Int16U duration		= servos[ index ].movingTime;
	
	if(servos[ index ].ease == E_SERVO_EASE_LINEAR)
	{
		servos[index].currentPosition = changeValue * (currentTime / duration) + startValue;
	}
	else if(servos[ index ].ease == E_SERVO_EASE_SINUS_IN)
	{
		servos[index].currentPosition = -changeValue * cos( M_PI_2 * (currentTime / duration)) + changeValue + startValue;
	}
	else if(servos[ index ].ease == E_SERVO_EASE_SINUS_OUT)
	{
		servos[index].currentPosition = changeValue * sin( M_PI_2 * (currentTime / duration)) + startValue;
	}
	else if(servos[ index ].ease == E_SERVO_EASE_SINUS_IN_OUT)
	{
		servos[index].currentPosition = (-changeValue/2) * (cos( M_PI * (currentTime / duration)) - 1 ) + startValue;
	}
	
	if(currentTime >= duration)
	{
		servos[index].currentPosition = servos[ index ].targetPosition;
		if(servos[index].callback != NULL)
		{
			servos[index].callback(index);
		}
	}
}
///////////////////////////////////////////ISR FUNCTIONS//////////////////////////////////////////
