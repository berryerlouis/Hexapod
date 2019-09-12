////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvTick.h"
#include "SrvHeartbeat.h"
#include "SrvFeeling.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
EFeelingLevel feel;
uint32_t prevMillisFeeling = 0U;
Boolean feelingSet = FALSE;
////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////

//Fonction d'initialisation
Boolean SrvFeelingInit ( void ) 
{
	feelingSet = FALSE;
	feel = FEELING_CALM;
	return TRUE;
}
//Fonction de dispatching d'evenements
void SrvFeelingUpdate (void)
{
	if( feel == FEELING_STRESS )
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
				feel = FEELING_CALM;
				SrvHeartbeatCalm();
			}
			else
			{
				volatile uint32_t delay = prevMillisFeeling - DrvTickGetTimeMs();
				SrvHeartbeatSetPeriod((uint8_t)(100 - (delay / 100)));
			}
		}
	}
}

void SrvFeelingSetFeeling (EFeelingLevel _feel)
{
	//only if stressed
	if( _feel == FEELING_STRESS )
	{
		feel = _feel;
		prevMillisFeeling = DrvTickGetTimeMs() + 10000U;
	}
}

