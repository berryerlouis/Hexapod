////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvLeg.h"
#include "Drv/DrvTick.h"

#include "SrvGait.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
Int32U prevMillisGait = 0U;

AxisFloat positionPath[3U];
////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////



//Fonction d'initialisation
Boolean SrvGaitInit ( void ) 
{
	positionPath[0].x = 0;
	positionPath[0].y = 65;
	positionPath[0].z = 20;
	positionPath[1].x = 45;
	positionPath[1].y = 65;
	positionPath[1].z = 0;
	positionPath[2].x = 60;
	positionPath[2].y = 65;
	positionPath[2].z = 20;
	return DrvLegInit();
	return TRUE;
}
static Int8U toto = 0;

//Fonction de dispatching d'evenements
void SrvGaitUpdate (void)
{
	DrvLegUpdate();
	if ((DrvTickGetTimeMs() - prevMillisGait) > 5000U)
	{
		if(DrvLegSetTarget(E_LEG_F_L,positionPath[toto],1000U))
		{		
			toto++;
			if(toto == 3) 
			{
				toto = 0;
			}
			//prevMillisGait = DrvTickGetTimeMs();
		}

	}
}
