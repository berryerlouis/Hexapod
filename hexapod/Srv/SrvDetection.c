////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Cmps/CmpVL53L0X.h"
#include "Cmps/CmpSRF04.h"
#include "Drv/DrvTick.h"

#include "SrvDetection.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
uint32_t prevMillisUpdateUltrason = 0UL;
E_US usIndexToSendEcho = E_US_0;
SDetection detection;
////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////

//Fonction d'initialisation
Boolean SrvDetectionInit ( void ) 
{
	usIndexToSendEcho = E_US_0;
	detection.notification = NULL;
	detection.usThreshold = US_THRESHOLD_DISTANCE;
	detection.lazerThreshold = LAZER_THRESHOLD_DISTANCE;
	detection.distance[E_US_0] = 0U;
	detection.distance[E_US_1] = 0U;
	detection.distance[E_LAZER_0 + 2U] = 0U;
	prevMillisUpdateUltrason = 0UL;
	if(TRUE == CmpVL53L0XInit(0.25, 20000))
	{
		//CmpVL53L0XSetTimeout(2);
		CmpVL53L0XStartContinuous(50);
	}
	
	return CmpSRF04Init();
}
//Fonction de dispatching d'evenements
void SrvDetectionUpdate (void) 
{
	//update every 20ms
	if ((DrvTickGetTimeMs() - prevMillisUpdateUltrason) > 50U)
	{
		if(CmpVL53L0XAvailable())
		{
			detection.distance[E_LAZER_0 + 2U] = CmpVL53L0XGetDistanceContinuous();
		}
		
		//CmpVL53L0XTimeoutOccurred();
		//send pulse to each ultrason module
		CmpSRF04SendPulse(usIndexToSendEcho);
		usIndexToSendEcho++;
		if(usIndexToSendEcho == E_NB_USS)
		{
			usIndexToSendEcho = E_US_0;
		}
		//get distance
		detection.distance[usIndexToSendEcho] = CmpSRF04GetDistance(usIndexToSendEcho);
		
		//if threshold is reach 
		if((detection.distance[usIndexToSendEcho] != 0) && (detection.distance[usIndexToSendEcho] < detection.usThreshold))
		{
			//notify 
			if(detection.notification != NULL)
			{
				detection.notification(usIndexToSendEcho,detection.distance[usIndexToSendEcho]);
			}
		}
		if((detection.distance[E_LAZER_0 + 2U] != 0) && (detection.distance[E_LAZER_0 + 2U] < detection.lazerThreshold))
		{
			if(detection.notification != NULL)
			{
				detection.notification(E_LAZER_0 + 2U,detection.distance[E_LAZER_0 + 2U]);
			} 
		}
		prevMillisUpdateUltrason = DrvTickGetTimeMs();
	}
}


SDetection* SrvDetectionGetStruct( void )
{
	return &detection;
}


Int16U SrvDetectionGetThresholdLazer( void )
{
	return detection.lazerThreshold;
}
void SrvDetectionSetThresholdLazer( Int16U lazerThreshold )
{
	detection.lazerThreshold = lazerThreshold;
}
Int16U SrvDetectionGetDistanceLazer(E_LAZER lazer)
{
	return detection.distance[lazer + 2U];
}

Int16U SrvDetectionGetThresholdUS( void )
{
	return detection.usThreshold;
}
void SrvDetectionSetThresholdUS( Int16U usThreshold )
{
	detection.usThreshold = usThreshold;
}

Int16U SrvDetectionGetDistanceUS(E_US us)
{
	return detection.distance[us];
}

void SrvDetectionSetNotification (SrvDetectionNotification cb) 
{
	detection.notification = cb;
}

/*
static Boolean SrvBodyUltrasonDetection (void)
{
	if ((DrvTickGetTimeMs() - prevMillisUpdateUS) > 100U)
	{
		if( ( SrvUltrasonReachThreshold(E_US_0) ) &&
		( SrvUltrasonReachThreshold(E_US_1) ))
		{
			Int8U step0 = (Int8U)((Int16U)(body.us->distance[E_US_0] * 15) / US_THRESHOLD_DISTANCE);
			Int8U step1 = (Int8U)((Int16U)(body.us->distance[E_US_1] * 15) / US_THRESHOLD_DISTANCE);
			if(step1 < step0) step0 = step1;
			
			if(body.feeling->state == FEELING_AGRESSIVE)
			{
				SrvBodyMoveSetRotationAndTranslation(step0,0,0,step0,0,-step0);
			}
			else if(body.feeling->state == FEELING_NEUTRAL)
			{
				SrvBodyMoveSetRotationAndTranslation(0,0,0,0,0,0);
			}
			else if(body.feeling->state == FEELING_FEAR)
			{
				SrvBodyMoveSetRotationAndTranslation(step0,0,0,-step0,0,+step0);
			}
		}
		else if( SrvUltrasonReachThreshold(E_US_0) )
		{
			Int8U step = (Int8U)(body.us->distance[E_US_0] * 15 / US_THRESHOLD_DISTANCE);
			if(body.feeling->state == FEELING_AGRESSIVE)
			{
				SrvBodyMoveSetRotationAndTranslation(step,0,step,step,0,-step);
			}
			else if(body.feeling->state == FEELING_NEUTRAL)
			{
				
				SrvBodyMoveSetRotationAndTranslation(0,0,step,0,0,0);
			}
			else if(body.feeling->state == FEELING_FEAR)
			{
				SrvBodyMoveSetRotationAndTranslation(step,0,step,-step,0,+step);
			}
		}
		else if( SrvUltrasonReachThreshold(E_US_1) )
		{
			Int8U step = (Int8U)(body.us->distance[E_US_1] * 15 / US_THRESHOLD_DISTANCE);
			if(body.feeling->state == FEELING_AGRESSIVE)
			{
				SrvBodyMoveSetRotationAndTranslation(step,0,-step,step,0,-step);
			}
			else if(body.feeling->state == FEELING_NEUTRAL)
			{
				SrvBodyMoveSetRotationAndTranslation(0,0,-step,0,0,0);
			}
			else if(body.feeling->state == FEELING_FEAR)
			{
				SrvBodyMoveSetRotationAndTranslation(step,0,-step,-step,0,+step);
			}
		}
		else
		{
			return FALSE;
		}
		SrvBodyMoveApplyRotationAndTranslation(250);
		prevMillisUpdateUS = DrvTickGetTimeMs();
		return TRUE;
	}
	
	return FALSE;
}
*/