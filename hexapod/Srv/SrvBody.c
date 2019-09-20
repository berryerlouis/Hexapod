////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvServo.h"
#include "Drv/DrvLeg.h"

#include "Srv/SrvBody.h"
#include "Srv/SrvWalk.h"
#include "Srv/SrvDisplay.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
extern struct SBody Body;


//Fonction d'initialisation
Boolean SrvBodyInit ( void ) 
{
	DrvLegInit();
	
	//init each servos of each leg
	for(Int8U i = 0U ; i < NB_LEGS ; i++ )
	{
		Body.legs[ i ] = DrvLegGetLeg( i );
	}
	Body.battery = SrvBatteryGetStruct();
	
	return TRUE;
}

//Fonction de dispatching d'evenements
void SrvBodyUpdate (void)
{
	
}
