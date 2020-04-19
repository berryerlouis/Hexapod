/*
 * SrvBodyMove.c
 *
 * Created: 18/04/2020 14:35:38
 *  Author: louis
 */ 
////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "../Drv/DrvTick.h"
#include "../Drv/DrvLeg.h"

#include "SrvBodyMove.h"
#include "SrvWalk.h"



////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
SBodyMove bodyMove;



//Fonction d'initialisation
Boolean SrvBodyMoveInit ( void )
{
	bodyMove.elevation = 0;
	bodyMove.groundSize = 0;
	
	bodyMove.roll = 0;
	bodyMove.pitch = 0;
	bodyMove.yaw = 0;
	bodyMove.initialized = FALSE;
	
	return TRUE;
}

//Fonction de dispatching d'evenements
void SrvBodyMoveUpdate (void)
{
	//wait for initialization of legs
	if(DrvLegIsInitialized())
	{
		//do it once
		if(bodyMove.initialized == FALSE)
		{
			bodyMove.initialized = TRUE;
			//set to position STAR 
			
			bodyMove.elevation = 50;
			bodyMove.groundSize = 80;
			//SrvBodyMoveSetBehavior(E_BODY_STAR,1500);
		}
	}
	//update legs position
	DrvLegUpdate();
}

SBodyMove *SrvBodyMoveGetStruct( void )
{
	return &bodyMove;
}

//Set Body Rotation
Boolean SrvBodyMoveSetRollPitchYaw ( float roll, float pitch, float yaw, uint16_t delay )
{
	bodyMove.roll = roll;
	bodyMove.pitch = pitch;
	bodyMove.yaw = yaw;
	DrvLegSetTarget(E_LEG_U_L,	DrvLegGetXFromCoxaAngle(15 + yaw	,bodyMove.groundSize)	,bodyMove.groundSize	,bodyMove.elevation + bodyMove.roll + bodyMove.pitch	,delay);
	DrvLegSetTarget(E_LEG_M_L,	DrvLegGetXFromCoxaAngle(yaw			,bodyMove.groundSize)	,bodyMove.groundSize	,bodyMove.elevation + bodyMove.pitch					,delay );
	DrvLegSetTarget(E_LEG_B_L,	-DrvLegGetXFromCoxaAngle(15 - yaw	,bodyMove.groundSize)	,bodyMove.groundSize	,bodyMove.elevation - bodyMove.roll + bodyMove.pitch	,delay);
	DrvLegSetTarget(E_LEG_U_R,	-DrvLegGetXFromCoxaAngle(15 - yaw	,bodyMove.groundSize)	,bodyMove.groundSize	,bodyMove.elevation + bodyMove.roll - bodyMove.pitch	,delay);
	DrvLegSetTarget(E_LEG_M_R,	DrvLegGetXFromCoxaAngle(yaw			,bodyMove.groundSize)	,bodyMove.groundSize	,bodyMove.elevation - bodyMove.pitch					,delay );
	DrvLegSetTarget(E_LEG_B_R,	DrvLegGetXFromCoxaAngle(15 + yaw	,bodyMove.groundSize)	,bodyMove.groundSize	,bodyMove.elevation - bodyMove.roll - bodyMove.pitch	,delay);
	return TRUE;
}

//Set vertical Rotation
Boolean SrvBodyMoveSetTranslation ( float x, float y, float z, uint16_t delay )
{
	bodyMove.x = x;
	bodyMove.y = y;
	bodyMove.z = z;
	DrvLegSetTarget(E_LEG_U_L,	DrvLegGetXFromCoxaAngle(15 - x		,bodyMove.groundSize - y)	,bodyMove.groundSize - y,bodyMove.elevation + z ,delay);
	DrvLegSetTarget(E_LEG_M_L,	DrvLegGetXFromCoxaAngle(-x			,bodyMove.groundSize - y)	,bodyMove.groundSize - y,bodyMove.elevation + z ,delay );
	DrvLegSetTarget(E_LEG_B_L,	-DrvLegGetXFromCoxaAngle(15 + x		,bodyMove.groundSize - y)	,bodyMove.groundSize - y,bodyMove.elevation + z ,delay);
	DrvLegSetTarget(E_LEG_U_R,	-DrvLegGetXFromCoxaAngle(15 - x		,bodyMove.groundSize + y)	,bodyMove.groundSize + y,bodyMove.elevation + z ,delay);
	DrvLegSetTarget(E_LEG_M_R,	DrvLegGetXFromCoxaAngle(x			,bodyMove.groundSize + y)	,bodyMove.groundSize + y,bodyMove.elevation + z ,delay );
	DrvLegSetTarget(E_LEG_B_R,	DrvLegGetXFromCoxaAngle(15 + x		,bodyMove.groundSize + y)	,bodyMove.groundSize + y,bodyMove.elevation + z ,delay);
	return TRUE;
}

//Get Behavior
EBodyBehavior SrvBodyMoveGetBehavior ( void )
{
	return bodyMove.behavior;
}
//Set Behavior
Boolean SrvBodyMoveSetBehavior ( EBodyBehavior pos, uint16_t delay )
{
	bodyMove.behavior = pos;
	//pause walk if needed
	SrvWalkSetWalk(E_WALK_STOP,delay);
	if(bodyMove.behavior == E_BODY_STOP)
	{
		//Stop position
		DrvLegSetTarget(E_LEG_U_L,	0,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_M_L,	0,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_B_L,	0,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_U_R,	0,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_M_R,	0,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_B_R,	0,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		return TRUE;
	}
	else if(bodyMove.behavior == E_BODY_STAR)
	{
		//star position
		DrvLegSetTarget(E_LEG_U_L,	DrvLegGetXFromCoxaAngle(15,bodyMove.groundSize)		,bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_M_L,	0													,bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_B_L,	-DrvLegGetXFromCoxaAngle(15,bodyMove.groundSize)	,bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_U_R,	-DrvLegGetXFromCoxaAngle(15,bodyMove.groundSize)	,bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_M_R,	0													,bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_B_R,	DrvLegGetXFromCoxaAngle(15,bodyMove.groundSize)		,bodyMove.groundSize,	bodyMove.elevation,	delay);
		return TRUE;
	}
	else if(bodyMove.behavior == E_BODY_STRAIGHT)
	{
		//straight position
		DrvLegSetTarget(E_LEG_U_L,	DrvLegGetXFromCoxaAngle(15,0)	,	0,	0,	delay);
		DrvLegSetTarget(E_LEG_M_L,	0								,	0,	0,	delay);
		DrvLegSetTarget(E_LEG_B_L,	-DrvLegGetXFromCoxaAngle(15,0)	,	0,	0,	delay);
		DrvLegSetTarget(E_LEG_U_R,	-DrvLegGetXFromCoxaAngle(15,0)	,	0,	0,	delay);
		DrvLegSetTarget(E_LEG_M_R,	0								,	0,	0,	delay);
		DrvLegSetTarget(E_LEG_B_R,	DrvLegGetXFromCoxaAngle(15,0)	,	0,	0,	delay);
		return TRUE;
	}
	return FALSE;
}
