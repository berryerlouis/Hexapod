////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "../Drv/DrvTick.h"
#include "../Drv/DrvLed.h"

#include "SrvBody.h"
#include "SrvPID.h"
#include "SrvIHM.h"



////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
static void SrvBodyUltrasonCallback (E_US us, uint16_t distance);
static void SrvBodyImuCallback (Int16S roll, Int16S pitch);
static Boolean SrvBodyUltrasonDetection (void);
static Boolean SrvBodyTouchDetection (void);
////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
SBodyPublic body;


//variables de timming
Int32U lastread_pid = 0U;
Int32U prevMillisUpdateBreath = 0U;
Int32U prevMillisUpdateUS = 0U;
Int32U prevMillisUpdateIMU = 0U;
Int8U prevMillisUpdateBreathStep = 0U;
//Fonction d'initialisation
Boolean SrvBodyInit ( void ) 
{
	body.legs = DrvLegGetLegs();
	
	body.battery = SrvBatteryGetStruct();
	
	body.imu = SrvImuSimpleGetSensor();
	SrvImuSimpleSetThreshold(IMU_THRESHOLD_ANGLE);	
	SrvImuSimpleSetCallback(SrvBodyImuCallback);
	
	body.us = SrvUltrasonGetStruct();
	SrvUltrasonSetThreshold(US_THRESHOLD_DISTANCE);
	SrvUltrasonSetCallbackThreshold(SrvBodyUltrasonCallback);
	
	body.move = SrvBodyMoveGetStruct();
	
	body.walk = SrvWalkgetStruct();
	
	body.feeling = SrvFeelingGetFeeling();
	
	body.initialized = FALSE;
	
	SrvPIDInit();
	SrvPIDSetValues( 0, 0.5F, 0.0F, 0.1F );
	SrvPIDSetValues( 1, 0.5F, 0.0F, 0.1F );
	
	
	return TRUE;
}

//Fonction de dispatching d'evenements
void SrvBodyUpdate (void)
{
	//at the beginning go to star position
	if((body.move->initialized) && (body.initialized == FALSE))
	{
		body.initialized = TRUE;
		SrvBodyMoveSetGroundSize(60,3000);
		SrvBodyMoveSetElevation(30,3000);
		SrvBodyMoveSetBehavior(E_BODY_STAR, 3000); 
	}
	
	//if initialize and stop so breath if not us detection
	if( (body.initialized == TRUE) && 
	    (body.walk->walking == E_WALK_STOP) )
	{
		Boolean checkTouch = FALSE;
		Boolean checkClosedObject = FALSE;
		checkClosedObject = SrvBodyUltrasonDetection();
		checkTouch = SrvBodyTouchDetection();
		//stop walk and move body if detection of ultrasound or touch detection
		if(!checkClosedObject && !checkTouch) 
		{	//no detection	
			if ((DrvTickGetTimeMs() - prevMillisUpdateBreath) > 4000U)
			{
				if(prevMillisUpdateBreathStep == 0)
				{
					SrvBodyMoveSetGroundSize(60,0);
					SrvBodyMoveSetElevation(45 ,0);
					SrvBodyMoveSetBehavior(SrvBodyMoveGetBehavior(), 2000);
					prevMillisUpdateBreathStep = 1;
				}
				else
				{
					SrvBodyMoveSetGroundSize(60,0);
					SrvBodyMoveSetElevation(50,0);
					SrvBodyMoveSetBehavior(SrvBodyMoveGetBehavior(), 2000);
					prevMillisUpdateBreathStep = 0;
				}
				prevMillisUpdateBreath = DrvTickGetTimeMs();
			}
		}
	}

	/*Int32U interval_pid = DrvTickGetTimeUs() - lastread_pid;
		
	Int16S errorRoll = SrvPIDCompute(0,bodyMove.imu->roll,bodyMove.x,interval_pid);
	Int16S errorPitch = SrvPIDCompute(1,bodyMove.imu->pitch,bodyMove.y,interval_pid);
	SrvBodySetPosition(errorRoll,errorPitch,200);
	//get loop update time
	lastread_pid = DrvTickGetTimeUs();	*/	
	
}


static Boolean SrvBodyUltrasonDetection (void)
{
	if ((DrvTickGetTimeMs() - prevMillisUpdateUS) > 100U)
	{
		if( ( SrvUltrasonReachThreshold(E_US_0) ) &&
			( SrvUltrasonReachThreshold(E_US_1) ))
		{
			if(body.feeling->state == FEELING_AGRESSIVE)
			{
				SrvBodyMoveSetRotationAndTranslation(15,0,0,15,0,-10,500);
			}
			else if(body.feeling->state == FEELING_NEUTRAL)
			{
				SrvBodyMoveSetRotationAndTranslation(15,0,0,0,0,0,500);
			}
			else if(body.feeling->state == FEELING_FEAR)
			{
				SrvBodyMoveSetRotationAndTranslation(15,0,0,-15,0,+10,500);
			}
			return TRUE;
		}
		else if( SrvUltrasonReachThreshold(E_US_1) )
		{
			if(body.feeling->state == FEELING_AGRESSIVE)
			{
				SrvBodyMoveSetRotationAndTranslation(15,0,15,15,0,-10,500);
			}
			else if(body.feeling->state == FEELING_NEUTRAL)
			{
				SrvBodyMoveSetRotationAndTranslation(15,0,15,0,0,0,500);
			}
			else if(body.feeling->state == FEELING_FEAR)
			{
				SrvBodyMoveSetRotationAndTranslation(15,0,15,-15,0,+10,500);
			}
			return TRUE;
		}
		else if( SrvUltrasonReachThreshold(E_US_0) )
		{
			if(body.feeling->state == FEELING_AGRESSIVE)
			{
				SrvBodyMoveSetRotationAndTranslation(15,0,-15,15,0,-10,500);
			}
			else if(body.feeling->state == FEELING_NEUTRAL)
			{
				SrvBodyMoveSetRotationAndTranslation(15,0,-15,0,0,0,500);
			}
			else if(body.feeling->state == FEELING_FEAR)
			{
				SrvBodyMoveSetRotationAndTranslation(15,0,-15,-15,0,+10,500);
			}
			return TRUE;
		}
		prevMillisUpdateUS = DrvTickGetTimeMs();
	}
	
	return FALSE;
}

static volatile Int16S touchDirection = 0;
static Boolean SrvBodyTouchDetection (void)
{
	Boolean isTouch = FALSE;
	Int8U touchThreshold = 5;
	if ((DrvTickGetTimeMs() - prevMillisUpdateIMU) > 10U)
	{
		if((abs(body.imu->roll) > touchThreshold) && (abs(body.imu->pitch) > touchThreshold) )
		{	
			isTouch = TRUE;
			if((body.imu->roll > 0 ) && (body.imu->pitch > 0 ))
			{
				touchDirection = 225;
			}
			else if((body.imu->roll < 0 ) && (body.imu->pitch > 0 ))
			{
				touchDirection = 315;
			}
			else if((body.imu->roll > 0 ) && (body.imu->pitch < 0 ))
			{
				touchDirection = 135;
			}
			else if((body.imu->roll < 0 ) && (body.imu->pitch < 0 ))
			{
				touchDirection = 45;
			}
		}
		else if(abs(body.imu->roll) > touchThreshold) 
		{
			isTouch = TRUE;
			if(body.imu->roll > 0 )
			{
				touchDirection = 180;
			}
			else if(body.imu->roll < 0 )
			{
				touchDirection = 0;
			}
		}
		else if(abs(body.imu->pitch) > touchThreshold)
		{
			isTouch = TRUE;
			if(body.imu->pitch > 0 )
			{
				touchDirection = 270;
			}
			else if(body.imu->pitch < 0 )
			{
				touchDirection = 90;
			}
		}
		prevMillisUpdateIMU = DrvTickGetTimeMs();
	}
	
	if( isTouch == TRUE )	
	{
//		SrvBodyMoveSetRotationAndTranslation((touchDirection*15/360),(touchDirection*15/360),(touchDirection*15/360),0,0,0,500);
	}
	
	return isTouch;
}

static void SrvBodyImuCallback (Int16S roll, Int16S pitch)
{
	SrvFeelingSetFeelingLevel(FEELING_STRESS);
}

static void SrvBodyUltrasonCallback (E_US us, uint16_t distance)
{
	//show led if threshold is reach
	if(us == E_US_0)
	{
		SrvWalkStopWalk();
		SrvIhmPlatformLeftLedTimeOn(E_LED_STATE_ON,500);
		
	}
	if(us == E_US_1)
	{
		SrvWalkStopWalk();
		SrvIhmPlatformRightLedTimeOn(E_LED_STATE_ON,500);
	}
}