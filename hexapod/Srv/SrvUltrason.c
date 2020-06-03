////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

//#include "Cmps/CmpVL53L0X.h"
#include "Cmps/CmpSRF04.h"
#include "Drv/DrvTick.h"

#include "SrvUltrason.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
uint32_t prevMillisUpdateUltrason = 0UL;
E_US usIndexToSendEcho = E_US_0;
SUltrason ultrason;
SrvUltrasonCallback callbackUSHit = NULL;
////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////

//Fonction d'initialisation
Boolean SrvUltrasonInit ( void ) 
{
	usIndexToSendEcho = E_US_0;
	ultrason.usthreshold = US_THRESHOLD_DISTANCE;
	ultrason.distance[E_US_0] = 0U;
	ultrason.distance[E_US_1] = 0U;
	prevMillisUpdateUltrason = 0UL;
	//CmpVL53L0XInit();
	return CmpSRF04Init();
}
//Fonction de dispatching d'evenements
void SrvUltrasonUpdate (void) 
{
	//update every 20ms
	if ((DrvTickGetTimeMs() - prevMillisUpdateUltrason) > 50U)
	{
		//send pulse to each ultrason module
		CmpSRF04SendPulse(usIndexToSendEcho);
		usIndexToSendEcho++;
		if(usIndexToSendEcho == E_NB_USS)
		{
			usIndexToSendEcho = E_US_0;
		}
		//get distance
		ultrason.distance[usIndexToSendEcho] = CmpSRF04GetDistance(usIndexToSendEcho);
		
		//if threshold is reach 
		if(ultrason.distance[usIndexToSendEcho] < ultrason.usthreshold)
		{
			//notify callback
			if(callbackUSHit != NULL)
			{
				callbackUSHit(usIndexToSendEcho, ultrason.distance[usIndexToSendEcho]);
			}
		}
		prevMillisUpdateUltrason = DrvTickGetTimeMs();
	}
}


SUltrason* SrvUltrasonGetStruct( void )
{
	return &ultrason;
}

Int16U SrvUltrasonGetDistance (E_US us)
{
	return CmpSRF04GetDistance(us);
}

// set threshold
Boolean SrvUltrasonSetThreshold( Int16U threshold )
{
	ultrason.usthreshold = threshold;
	return TRUE;
}

// set threshold
Boolean SrvUltrasonReachThreshold( E_US us )
{
	if( SrvUltrasonGetDistance(us) < ultrason.usthreshold )
	{
		return TRUE;	
	}
	return FALSE;
}

// get threshold
Int16U SrvUltrasonGetThreshold( void )
{
	return ultrason.usthreshold;
}


void SrvUltrasonSetCallbackThreshold ( SrvUltrasonCallback cb ) 
{
	callbackUSHit = cb;
}