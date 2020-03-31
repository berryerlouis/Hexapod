////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Cmps/CmpSRF04.h"
#include "Drv/DrvTick.h"

#include "SrvIhm.h"
#include "SrvFeeling.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define US_THRESHOLD_DISTANCE	45U		// 45 cm
////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
static void SrvUltrasonShowLeds (void) ;

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
uint32_t prevMillisUpdateUltrason = 0UL;
Boolean usToggle = FALSE;
uint8_t usthreshold = US_THRESHOLD_DISTANCE;
////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////

//Fonction d'initialisation
Boolean SrvUltrasonInit ( void ) 
{
	usToggle = FALSE;
	usthreshold = US_THRESHOLD_DISTANCE;
	prevMillisUpdateUltrason = 0UL;
	return CmpSRF04Init();
}
//Fonction de dispatching d'evenements
void SrvUltrasonUpdate (void) 
{
	//update every 20ms
	if ((DrvTickGetTimeMs() - prevMillisUpdateUltrason) > 50U)
	{
		if(usToggle)
		{
			usToggle = FALSE;
			CmpSRF04SendPulse(E_US_1);
		}
		else 
		{
			usToggle = TRUE;
			CmpSRF04SendPulse(E_US_0);
		}
		prevMillisUpdateUltrason = DrvTickGetTimeMs();
		
		//show led if threshold is reach
		SrvUltrasonShowLeds();
	}
}

uint16_t SrvUltrasonGetDistance (E_US us)
{
	return CmpSRF04GetDistance(us);
}

// set threshold
Boolean SrvUltrasonSetThreshold( uint8_t threshold )
{
	usthreshold = threshold;
	return TRUE;
}
// get threshold
uint8_t SrvUltrasonGetThreshold( void )
{
	return usthreshold;
}


static void SrvUltrasonShowLeds (void) 
{
	if(CmpSRF04GetDistance(E_US_0) <= US_THRESHOLD_DISTANCE)
	{
		SrvIhmPlatformLedLeft(E_LED_STATE_ON);
		SrvFeelingSetFeeling(FEELING_STRESS);
	}
	else
	{
		SrvIhmPlatformLedLeft(E_LED_STATE_OFF);
		SrvFeelingSetFeeling(FEELING_CALM);
	}
		
	if(CmpSRF04GetDistance(E_US_1) <= US_THRESHOLD_DISTANCE)
	{
		SrvIhmPlatformLedRight(E_LED_STATE_ON);
		SrvFeelingSetFeeling(FEELING_STRESS);
	}
	else
	{
		SrvIhmPlatformLedRight(E_LED_STATE_OFF);
		SrvFeelingSetFeeling(FEELING_CALM);
	}
}