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
		servos[ index ].ease = E_SERVO_EASE_SINUS_IN_OUT;
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
			//if not already set
			if(servos[ index ].targetPosition != angle)
			{
				servos[ index ].startPosition = servos[ index ].currentPosition;
				servos[ index ].targetPosition = angle;
				servos[ index ].startTime = DrvTickGetTimeMs();
				//compute minimum time to reach target
				Int16U delta = (Int16U)((Int16U)abs((Int16U)servos[ index ].targetPosition - (Int16U)servos[ index ].currentPosition));
				Int16U min_time = (delta * 1000U) / SERVO_DEG_PER_SEC;
				if(time <= min_time)
				{
					time = min_time;
				}
				servos[ index ].movingTime = time;
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
	for (Int8U index = 0U; index < NB_SERVOS ; index++)
	{
		DrvServoComputeAngle(index);
	
		Int16S angle = servos[index].currentPosition;
	
		//convert to 0� to 180,0�
		angle += 90;
		angle = SetRangeInt16(angle, SERVO_ANGLE_MIN, SERVO_ANGLE_MAX, 114, 522);
	
		//send to PCA9685
		if(index > 8)
		{
			CmpPCA9685SetPWM(PCA9685_ADDRESS_0, index - 9, 0, angle);
		}
		else
		{
			CmpPCA9685SetPWM(PCA9685_ADDRESS_1, index, 0, angle);
		}
	}
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
	//if needed
	if( DrvServoCheckPosition(index) == FALSE )
	{
		float currentTime	= DrvTickGetTimeMs() - servos[ index ].startTime;
		float startValue	= servos[ index ].startPosition;
		float changeValue	= servos[ index ].targetPosition - startValue;
		float duration		= servos[ index ].movingTime;
		
		if(servos[ index ].ease == E_SERVO_EASE_LINEAR)
		{
			servos[index].currentPosition = round(changeValue * (currentTime / duration) + startValue);
		}
		else if(servos[ index ].ease == E_SERVO_EASE_SINUS_IN)
		{
			servos[index].currentPosition = round(-changeValue * cos( M_PI_2 * (currentTime / duration)) + changeValue + startValue);
		}
		else if(servos[ index ].ease == E_SERVO_EASE_SINUS_OUT)
		{
			servos[index].currentPosition = round(changeValue * sin( M_PI_2 * (currentTime / duration)) + startValue);
		}
		else if(servos[ index ].ease == E_SERVO_EASE_SINUS_IN_OUT)
		{
			servos[index].currentPosition = round((-changeValue/2) * (cos( M_PI * (currentTime / duration)) - 1 ) + startValue);
		}
		
		if(currentTime > duration)
		{
			servos[index].currentPosition = servos[ index ].targetPosition;
		}
		else if(currentTime == duration)
		{
			if(servos[index].callback != NULL)
			{
				servos[index].callback(index);
			}
		}
	}
}
///////////////////////////////////////////ISR FUNCTIONS//////////////////////////////////////////
