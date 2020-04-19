////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "../Drv/DrvTick.h"

#include "SrvBody.h"
#include "SrvBodyMove.h"
#include "SrvPID.h"
#include "SrvFeeling.h"
#include "SrvIhm.h"



////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
static void SrvBodyUltrasonCallback (E_US us, uint16_t distance);
////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
SBody body;


//variables de timming
Int32U lastread_pid = 0U;
Int32U prevMillisUpdateBreath = 0U;
Int8U prevMillisUpdateBreathStep = 0U;
//Fonction d'initialisation
Boolean SrvBodyInit ( void ) 
{
	body.legs = DrvLegGetLegs();
	
	body.battery = SrvBatteryGetStruct();
	
	body.imu = SrvImuSimpleGetSensor();
	
	body.us = SrvUltrasonGetStruct();
	SrvUltrasonSetThreshold(US_THRESHOLD_DISTANCE);
	SrvUltrasonSetCallbackThreshold(SrvBodyUltrasonCallback);
	
	body.move = SrvBodyMoveGetStruct();
	
	body.walk = SrvWalkgetStruct();
	
	body.initialized = FALSE;
	
	SrvPIDInit();
	SrvPIDSetValues( 0, 0.5F, 0.0F, 0.1F );
	SrvPIDSetValues( 1, 0.5F, 0.0F, 0.1F );
	
	
	return TRUE;
}

//Fonction de dispatching d'evenements
void SrvBodyUpdate (void)
{
	if((body.move->initialized) && (body.walk->walking == E_WALK_STOP))
	{
		/*if ((DrvTickGetTimeMs() - prevMillisUpdateBreath) > 1000U)
		{
			if(prevMillisUpdateBreathStep == 0)
			{
				SrvBodyMoveSetElevation(45,1000);
				prevMillisUpdateBreathStep = 1;
			}
			else
			{
				SrvBodyMoveSetElevation(50,1000);
				prevMillisUpdateBreathStep = 0;
			}
			prevMillisUpdateBreath = DrvTickGetTimeMs();
		}*/
	}

		
	/*Int32U interval_pid = DrvTickGetTimeUs() - lastread_pid;
		
	Int16S errorRoll = SrvPIDCompute(0,bodyMove.imu->roll,bodyMove.x,interval_pid);
	Int16S errorPitch = SrvPIDCompute(1,bodyMove.imu->pitch,bodyMove.y,interval_pid);
	SrvBodySetPosition(errorRoll,errorPitch,200);
	//get loop update time
	lastread_pid = DrvTickGetTimeUs();	*/	
	
}


static void SrvBodyUltrasonCallback (E_US us, uint16_t distance)
{
	//show led if threshold is reach
	if(us == E_US_0)
	{
		SrvIhmPlatformLeftLedTimeOn(E_LED_STATE_ON,500);
	}
	if(us == E_US_1)
	{
		SrvIhmPlatformRightLedTimeOn(E_LED_STATE_ON,500);
	}
	
	
	//SrvWalkSetWalk(E_WALK_STOP, 1000U);
	//SrvBodyMoveSetBehavior(E_BODY_STAR,1000);
}