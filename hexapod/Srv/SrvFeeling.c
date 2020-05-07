////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvTick.h"
#include "SrvHeartbeat.h"
#include "SrvFeeling.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////

SFeeling feeling;
Boolean feelingSet = FALSE;
Int32U prevMillisFeeling = 0U;
////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////

//Fonction d'initialisation
Boolean SrvFeelingInit ( void ) 
{
	feeling.stressedPeriod = 10000;
	feeling.feeling = FEELING_CALM;
	feeling.state = FEELING_NEUTRAL;
	feelingSet = FALSE;
	return TRUE;
}
//Fonction de dispatching d'evenements
void SrvFeelingUpdate (void)
{
	if( feeling.feeling == FEELING_STRESS )
	{
		if(feelingSet == FALSE)
		{
			feelingSet = TRUE;
		}
		else
		{
			if(DrvTickGetTimeMs() >= prevMillisFeeling)
			{
				feelingSet = FALSE;
				feeling.feeling = FEELING_CALM;
				SrvHeartbeatCalm();
			}
			else
			{
				SrvHeartbeatSetPeriod((uint8_t)(100 - (prevMillisFeeling - DrvTickGetTimeMs()) / 100));
			}
		}
	}
}

void SrvFeelingSetFeelingLevel (EFeelingLevel feel)
{
	//only if stressed
	if( feel == FEELING_STRESS )
	{
		feeling.feeling = feel;
		prevMillisFeeling = DrvTickGetTimeMs() + feeling.stressedPeriod;
	}
}
void SrvFeelingSetFeelingStatus (EFeelingState state)
{
	feeling.state = state;
}
SFeeling *SrvFeelingGetFeeling ( void ) 
{
	return &feeling;
}


