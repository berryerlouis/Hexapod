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
	
	
	return TRUE;
}

//Fonction de dispatching d'evenements
void SrvBodyUpdate (void)
{
	SrvWalkUpdate();
}


Boolean SrvBodySetPosition( E_BODY_POSITION position, uint16_t delay)
{
	return TRUE;
}

Boolean SrvBodySetWalk( E_WALK walk, uint16_t delay )
{
	if(SrvWalkSetWalk(walk,delay))
	{
		if(walk == E_WALK_FW)		SrvDisplaySetDirection(ARROW_UP);
		else if(walk == E_WALK_RV)		SrvDisplaySetDirection(ARROW_DOWN);
		else SrvDisplaySetDirection(ARROW_CENTER);
		return TRUE;
	}
	
	return FALSE;
}

Boolean SrvBodySetGait( E_GAIT gait, uint16_t delay )
{
	return SrvWalkSetGait(gait,delay);
}