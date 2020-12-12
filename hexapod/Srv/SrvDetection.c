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
E_US usIndexToSendEcho = E_ULTRASON_0;
SDetection detection;
////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////

//Fonction d'initialisation
Boolean SrvDetectionInit ( void ) 
{
	usIndexToSendEcho = E_ULTRASON_0;
	detection.notification = NULL;
	detection.usThreshold = US_THRESHOLD_DISTANCE;
	detection.lazerThreshold = LAZER_THRESHOLD_DISTANCE;
	detection.distance[E_DIST_SENSOR_0] = 0U;
	detection.detect[E_DIST_SENSOR_0] = FALSE;
	detection.distance[E_DIST_SENSOR_1] = 0U;
	detection.detect[E_DIST_SENSOR_1] = FALSE;
	detection.distance[E_DIST_SENSOR_2] = 0U;
	detection.detect[E_DIST_SENSOR_2 + 2U] = FALSE;
	
	prevMillisUpdateUltrason = 0UL;
	if(TRUE == CmpVL53L0XInit(0.25, 20000))
	{
		CmpVL53L0XStartContinuous(50);
	}
	
	return CmpSRF04Init();
}
//Fonction de dispatching d'evenements
void SrvDetectionUpdate (void) 
{
	//update every 50ms
	if ((DrvTickGetTimeMs() - prevMillisUpdateUltrason) > 50U)
	{
		if(CmpVL53L0XAvailable())
		{
			detection.distance[E_DIST_SENSOR_2] = CmpVL53L0XGetDistanceContinuous();
		}
		
		//CmpVL53L0XTimeoutOccurred();
		//send pulse to each ultrason module
		CmpSRF04SendPulse(usIndexToSendEcho);
		//get distance
		detection.distance[usIndexToSendEcho] = CmpSRF04GetDistance(usIndexToSendEcho);
		
		//if threshold is reach n US sensors
		if((detection.distance[usIndexToSendEcho] != 0) && (detection.distance[usIndexToSendEcho] < detection.usThreshold))
		{ 
			//only once
			if(detection.detect[usIndexToSendEcho] == FALSE)
			{
				detection.detect[usIndexToSendEcho] = TRUE;
				if(detection.notification != NULL)
				{
					detection.notification(detection.detect[usIndexToSendEcho]);
				}
			}
		}
		else
		{
			//only once
			if(detection.detect[usIndexToSendEcho] == TRUE)
			{
				detection.detect[usIndexToSendEcho] = FALSE;
				if(detection.notification != NULL)
				{
					detection.notification(detection.detect[usIndexToSendEcho]);
				}
			}
		}
		
		//if threshold is reach on lazer sensors
		if((detection.distance[E_DIST_SENSOR_2] != 0) && (detection.distance[E_DIST_SENSOR_2] < detection.lazerThreshold))
		{
			//only once
			if(detection.detect[E_DIST_SENSOR_2] == FALSE)
			{
				detection.detect[E_DIST_SENSOR_2] = TRUE;
				if(detection.notification != NULL)
				{
					detection.notification(detection.detect[E_DIST_SENSOR_2]);
				}
			}
		}
		else
		{
			//only once
			if(detection.detect[E_DIST_SENSOR_2] == TRUE)
			{
				detection.detect[E_DIST_SENSOR_2] = FALSE;
				if(detection.notification != NULL)
				{
					detection.notification(detection.detect[E_DIST_SENSOR_2]);
				}
			}
		}
		
		usIndexToSendEcho++;
		if(usIndexToSendEcho == E_NB_ULTRASONS)
		{
			usIndexToSendEcho = E_ULTRASON_0;
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

