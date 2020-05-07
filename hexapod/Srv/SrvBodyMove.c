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


//Set vertical Rotation
Boolean SrvBodyMoveSetRotationAndTranslation ( float roll, float pitch, float yaw, float x, float y, float z, uint16_t delay )
{
	bodyMove.roll = roll;
	bodyMove.pitch = pitch;
	bodyMove.yaw = yaw;
	bodyMove.x = x;
	bodyMove.y = y;
	bodyMove.z = z;
	DrvLegSetTarget(E_LEG_F_L,	DrvLegGetXFromCoxaAngle(15 + bodyMove.yaw - bodyMove.x	,bodyMove.groundSize - bodyMove.y)	,bodyMove.groundSize - bodyMove.y,	bodyMove.elevation + bodyMove.roll + bodyMove.pitch	 + bodyMove.z ,delay);
	DrvLegSetTarget(E_LEG_M_L,	DrvLegGetXFromCoxaAngle(bodyMove.yaw - bodyMove.x		,bodyMove.groundSize - bodyMove.y)	,bodyMove.groundSize - bodyMove.y,	bodyMove.elevation + bodyMove.pitch					 + bodyMove.z ,delay);
	DrvLegSetTarget(E_LEG_R_L,	-DrvLegGetXFromCoxaAngle(15 - bodyMove.yaw + bodyMove.x	,bodyMove.groundSize - bodyMove.y)	,bodyMove.groundSize - bodyMove.y,	bodyMove.elevation - bodyMove.roll + bodyMove.pitch	 + bodyMove.z ,delay);
	DrvLegSetTarget(E_LEG_F_R,	-DrvLegGetXFromCoxaAngle(15 - bodyMove.yaw - bodyMove.x	,bodyMove.groundSize + bodyMove.y)	,bodyMove.groundSize + bodyMove.y,	bodyMove.elevation + bodyMove.roll - bodyMove.pitch	 + bodyMove.z ,delay);
	DrvLegSetTarget(E_LEG_M_R,	DrvLegGetXFromCoxaAngle(bodyMove.yaw + bodyMove.x		,bodyMove.groundSize + bodyMove.y)	,bodyMove.groundSize + bodyMove.y,	bodyMove.elevation - bodyMove.pitch					 + bodyMove.z ,delay);
	DrvLegSetTarget(E_LEG_R_R,	DrvLegGetXFromCoxaAngle(15 + bodyMove.yaw + bodyMove.x	,bodyMove.groundSize + bodyMove.y)	,bodyMove.groundSize + bodyMove.y,	bodyMove.elevation - bodyMove.roll - bodyMove.pitch	 + bodyMove.z ,delay);
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
	if(bodyMove.behavior == E_BODY_STOP)
	{
		//Stop position
		DrvLegSetTarget(E_LEG_F_L,	0,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_M_L,	0,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_R_L,	0,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_F_R,	0,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_M_R,	0,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_R_R,	0,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		return TRUE;
	}
	else if(bodyMove.behavior == E_BODY_STAR)
	{
		//star position
		DrvLegSetTarget(E_LEG_F_L,	DrvLegGetXFromCoxaAngle(15,bodyMove.groundSize)		,bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_M_L,	0													,bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_R_L,	-DrvLegGetXFromCoxaAngle(15,bodyMove.groundSize)	,bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_F_R,	-DrvLegGetXFromCoxaAngle(15,bodyMove.groundSize)	,bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_M_R,	0													,bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_R_R,	DrvLegGetXFromCoxaAngle(15,bodyMove.groundSize)		,bodyMove.groundSize,	bodyMove.elevation,	delay);
		return TRUE;
	}
	else if(bodyMove.behavior == E_BODY_STRAIGHT)
	{
		bodyMove.groundSize = 0;
		bodyMove.elevation = 0;
		//straight position
		DrvLegSetTarget(E_LEG_F_L,	DrvLegGetXFromCoxaAngle(15,0)	,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_M_L,	0								,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_R_L,	-DrvLegGetXFromCoxaAngle(15,0)	,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_F_R,	-DrvLegGetXFromCoxaAngle(15,0)	,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_M_R,	0								,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_R_R,	DrvLegGetXFromCoxaAngle(15,0)	,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		return TRUE;
	}
	return FALSE;
}

//Set Ground Size
Boolean SrvBodyMoveSetGroundSize ( float groundSize, uint16_t delay )
{
	bodyMove.groundSize = groundSize;
	return TRUE;
}
//Set Elevation
Boolean SrvBodyMoveSetElevation ( float elevation, uint16_t delay )
{
	bodyMove.elevation = elevation;
	return TRUE;
}