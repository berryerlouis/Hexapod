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
static float DrvServoComputeAngle ( Int8U index );
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
		servos[ index ].enable = FALSE;
		servos[ index ].currentPosition = 0;
		servos[ index ].startPosition = 0;
		servos[ index ].targetPosition = 0;
		servos[ index ].movingTime = 0U;
		servos[ index ].startTime = 0U;
		servos[ index ].callback = NULL;
		servos[ index ].offset = 0;
		servos[ index ].min = SERVO_ANGLE_MIN;
		servos[ index ].mid = SERVO_ANGLE_MID;
		servos[ index ].max = SERVO_ANGLE_MAX;
	}
	
	//initialize the 2 components PCA9685
	CmpPCA9685Init(PCA9685_ADDRESS_0);
	CmpPCA9685Init(PCA9685_ADDRESS_1);
	//set default PWM frequency to 60Hz
	CmpPCA9685SetPWMFreq(PCA9685_ADDRESS_0, SERVO_FREQ);
	CmpPCA9685SetPWMFreq(PCA9685_ADDRESS_1, SERVO_FREQ);

	return TRUE;
}

// move the selected servo to the desired position [-90,90] and during the desired time
Boolean DrvServoSetTarget( Int8U index, Int16S angle, Int16U time)
{
	//check index
	if(( index < NB_SERVOS ) && (servos[ index ].enable == TRUE))
	{
		//if between limits
		if(( angle >= servos[ index ].min ) && ( angle <= servos[ index ].max ))
		{
			//if not already set, the servo need to move
			if(servos[ index ].targetPosition != angle)
			{
				//set position and target
				servos[ index ].startPosition	= servos[ index ].currentPosition;
				servos[ index ].targetPosition	= angle;
				//compute the delta
				Int16S delta = servos[ index ].targetPosition - servos[ index ].startPosition;
				//compute minimum time for this delta
				Int16U durationMin = (Int16U)ABS((SERVO_SPEED_MSEC_PER_DEG * (float)delta));
				//set the delay min for the servo to reaches its position if time < durationMin
				if( durationMin > time )
				{
					servos[ index ].movingTime = durationMin - 1;
				}
				else
				{
					servos[ index ].movingTime = time;
				}
				servos[ index ].startTime = DrvTickGetTimeMs();
			}
			return TRUE;
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

// activate or not the desired servo
Boolean DrvServoActivate( Int8U index, Boolean enable)
{
	servos[ index ].enable = enable;
	return TRUE;
}

// get the target of the selected servo
Int16S DrvServoGetTarget( Int8U index )
{
	return servos[ index ].targetPosition;
}

//check if servo reaches target
Boolean DrvServoReachesPosition( Int8U index )
{
	if( servos[index].currentPosition != servos[index].targetPosition ) 
	{
		return FALSE;
	}
	return TRUE;
}

static float angle ;
Int32U prevMillisServoUpdate = 0U;
SPCA9685Pwm pwm[NB_SERVOS];
void DrvServoUpdate ( void )
{
	//update every 10 ms
	if ((DrvTickGetTimeMs() - prevMillisServoUpdate) > 10U)
	{
		//fill the pwm for each servos
		for (Int8U index = 0U; index < NB_SERVOS ; index++)
		{
			//only if servo activate and position need to move to target
			if(servos[ index ].enable == TRUE)
			{
				angle = (float)servos[index].currentPosition;
				//if servo reach its target
				if((DrvServoReachesPosition(index) == TRUE))
				{
					//send callabck once if different of NULL
					if(servos[ index ].callback != NULL)
					{
						servos[index].callback(index);
					}
					servos[index].startPosition = servos[ index ].targetPosition;
				}
				else
				{
					//compute new angle step 
					angle = DrvServoComputeAngle(index);
				}
				
				//convert -90deg to 90deg => 0deg to 180deg
				angle += 90;
				angle = SetRange(angle, SERVO_ANGLE_MIN, SERVO_ANGLE_MAX, 195.0, 480.0);
				pwm[index].on = 0U;
				pwm[index].off = (Int16U)angle;
			}
			else
			{
				pwm[index].on = 0U;
				pwm[index].off = 4096;
			}
		}
		//send to each component
		CmpPCA9685SetAllPWM(PCA9685_ADDRESS_1,pwm,NB_SERVOS/2U);
		CmpPCA9685SetAllPWM(PCA9685_ADDRESS_0,&pwm[NB_SERVOS/2U],NB_SERVOS/2U);
		//for next time
		prevMillisServoUpdate = DrvTickGetTimeMs();
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

float DrvServoComputeAngle ( Int8U index )
{	
	static Int32U currentTime;
	currentTime	= abs(DrvTickGetTimeMs() - servos[ index ].startTime);
	float startValue	= (float)servos[ index ].startPosition;
	float changeValue	= (float)servos[ index ].targetPosition - startValue;
	float duration		= (float)servos[ index ].movingTime;
	
	//servos[index].currentPosition = (Int16S)round(changeValue * (float)(currentTime / duration) + startValue);

	//if(currentTime >= duration)
	{
		servos[index].currentPosition = servos[index].targetPosition;
	}
	return servos[index].currentPosition;
}
///////////////////////////////////////////ISR FUNCTIONS//////////////////////////////////////////
