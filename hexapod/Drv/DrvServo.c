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

#define NB_SERVOS_ON_PCA9685_ADDRESS_0	10U //9 of legs on right side + 1 for head
#define NB_SERVOS_ON_PCA9685_ADDRESS_1	9U  //9 of legs on left side

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
static SServo servos[NB_SERVOS];
/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
// Initialization of Servo
Boolean DrvServoInit( void )
{
	//set servos to default value
	for (Int8U index = 0U; index < NB_SERVOS ; index++)
	{
		servos[ index ].id = index;
		servos[ index ].enable = FALSE;
		servos[ index ].currentPosition = 0;
		servos[ index ].offset = 0;
		servos[ index ].min = SERVO_ANGLE_MIN;
		servos[ index ].mid = SERVO_ANGLE_MID;
		servos[ index ].max = SERVO_ANGLE_MAX;
	}
	
	//initialize the 2 components PCA9685
	CmpPCA9685Init(PCA9685_ADDRESS_0, SERVO_FREQ);
	CmpPCA9685Init(PCA9685_ADDRESS_1, SERVO_FREQ);

	return TRUE;
}

// move the selected servo to the desired position [-900,900] and during the desired time
Boolean DrvServoSetTarget( Int8U index, Int16S angle, Int16U time)
{
	//check index
	if(( index < NB_SERVOS ) && (servos[ index ].enable == TRUE))
	{
		//if between limits
		if(( angle >= servos[ index ].min ) && ( angle <= servos[ index ].max ))
		{
			servos[ index ].currentPosition = angle;
			return TRUE;
		}
	}
	return FALSE;
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
	return servos[ index ].currentPosition;
}


//permits to send not in a same time to the 2 PCA9685
Boolean switchPCA9685 = FALSE;
//call every loop in order to send the new position to PCA9585
void DrvServoUpdate ( void )
{ 
	float angle = 0;
	//structure of PCA9585 tab representing pwm
	SPCA9685Pwm *pwm;
	//fill the pwm for each servos
	for (Int8U index = 0U; index < NB_SERVOS ; index++)
	{
		//get structure of current index for the 2 PCA9685 components
		//the PCA9685_ADDRESS_1 has 9 servos index 0 up to 8
		//the PCA9685_ADDRESS_0 has 10 servos index 9 up to 19
		if(index < NB_SERVOS_ON_PCA9685_ADDRESS_1)
		{
			pwm = CmpPCA9685GetPwmTab(PCA9685_ADDRESS_1, index);
		}
		else
		{
			pwm = CmpPCA9685GetPwmTab(PCA9685_ADDRESS_0, index - NB_SERVOS_ON_PCA9685_ADDRESS_1);
		}
		//always equals to 0
		pwm->on = 0U;
		//only if servo activate and position need to move to target
		if(servos[ index ].enable == TRUE)
		{
			//get current angle
			angle = servos[index].currentPosition;
			//convert -900deg to 900deg => 0deg to 1800deg
			angle += 900;
			//do range for PCA9585
			pwm->off = (Int16U)(SetRange(angle, SERVO_ANGLE_MIN, SERVO_ANGLE_MAX, 195.0, 480.0));
		}
		else
		{
			//servo disable set output to LOW
			pwm->off = 4096;
		}
	}
	//send to each component
	if(switchPCA9685)
	{
		switchPCA9685 = FALSE;
		//there are 9 servos on left side legs
		CmpPCA9685SendBuffer(PCA9685_ADDRESS_1, NB_SERVOS_ON_PCA9685_ADDRESS_1);
	}
	else
	{
		switchPCA9685 = TRUE;
		//there are 9 servos on right side legs + 1 servo for the head
		CmpPCA9685SendBuffer(PCA9685_ADDRESS_0,NB_SERVOS_ON_PCA9685_ADDRESS_0);
	}
}

// get the servo structure pointer at index
SServo* DrvServoGetStruture( Int8U index )
{
	//check index
	if( index < NB_SERVOS )
	{
		//return right servo structure
		return &servos[ index ];
	}
	//return null if wrong
	return NULL;
}

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

///////////////////////////////////////////ISR FUNCTIONS//////////////////////////////////////////
