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
	
	return TRUE;
}


SBodyMove *SrvBodyMoveGetStruct( void )
{
	return &bodyMove;
}

//Set vertical Rotation
SBodyLegMove SrvBodyMoveComputeLegRotationAndTranslation ( ELeg legId )
{
	SBodyLegMove legMove;
	if(legId == E_LEG_F_L)
	{
		legMove.x = DrvLegGetXFromCoxaAngle(15 + bodyMove.yaw - bodyMove.x	,bodyMove.groundSize - bodyMove.y);
		legMove.y = bodyMove.groundSize - bodyMove.y;
		legMove.z = bodyMove.elevation + bodyMove.roll + bodyMove.pitch	 + bodyMove.z ;
	}
	else if(legId == E_LEG_M_L)
	{
		legMove.x = DrvLegGetXFromCoxaAngle(bodyMove.yaw - bodyMove.x		,bodyMove.groundSize - bodyMove.y);
		legMove.y = bodyMove.groundSize - bodyMove.y;
		legMove.z = bodyMove.elevation + bodyMove.pitch					 + bodyMove.z ;
	}
	else if(legId == E_LEG_R_L)
	{
		legMove.x = -DrvLegGetXFromCoxaAngle(15 - bodyMove.yaw + bodyMove.x	,bodyMove.groundSize - bodyMove.y);
		legMove.y = bodyMove.groundSize - bodyMove.y;
		legMove.z = bodyMove.elevation - bodyMove.roll + bodyMove.pitch	 + bodyMove.z ;
	}
	else if(legId == E_LEG_F_R)
	{
		legMove.x = -DrvLegGetXFromCoxaAngle(15 - bodyMove.yaw - bodyMove.x	,bodyMove.groundSize + bodyMove.y);
		legMove.y = bodyMove.groundSize + bodyMove.y;
		legMove.z = bodyMove.elevation + bodyMove.roll - bodyMove.pitch	 + bodyMove.z ;
	}
	else if(legId == E_LEG_M_R)
	{
		legMove.x = DrvLegGetXFromCoxaAngle(bodyMove.yaw + bodyMove.x		,bodyMove.groundSize + bodyMove.y);
		legMove.y = bodyMove.groundSize + bodyMove.y;
		legMove.z = bodyMove.elevation - bodyMove.pitch					 + bodyMove.z ;
	}
	else if(legId == E_LEG_R_R)
	{
		legMove.x = DrvLegGetXFromCoxaAngle(15 + bodyMove.yaw + bodyMove.x	,bodyMove.groundSize + bodyMove.y);
		legMove.y = bodyMove.groundSize + bodyMove.y;
		legMove.z = bodyMove.elevation - bodyMove.roll - bodyMove.pitch	 + bodyMove.z ;
	}
	return legMove;
}
	
//Set vertical Rotation
Boolean SrvBodyMoveSetRotationAndTranslation ( float roll, float pitch, float yaw, float x, float y, float z)
{
	bodyMove.roll = roll;
	bodyMove.pitch = pitch;
	bodyMove.yaw = yaw;
	bodyMove.x = x;
	bodyMove.y = y;
	bodyMove.z = z;
	return TRUE;
}

//Set vertical Rotation
Boolean SrvBodyMoveApplyRotationAndTranslation ( uint16_t delay )
{
	SBodyLegMove legMove;
	for(Int8U indexLeg = 0U ; indexLeg < E_NB_LEGS ; indexLeg++ )
	{
		legMove = SrvBodyMoveComputeLegRotationAndTranslation(indexLeg);
		//DrvLegSetTarget(indexLeg,	legMove.x , legMove.y, legMove.z , delay);
	}
	return TRUE;
}

//Get Behavior
EBodyBehavior SrvBodyMoveGetPosition ( void )
{
	return bodyMove.behavior;
}
//Set Behavior
Boolean SrvBodyMoveSetPosition ( EBodyBehavior pos, uint16_t delay )
{
	bodyMove.behavior = pos;
	if(bodyMove.behavior == E_BODY_STOP)
	{
		//Stop position
		/*DrvLegSetTarget(E_LEG_F_L,	0,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_M_L,	0,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_R_L,	0,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_F_R,	0,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_M_R,	0,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_R_R,	0,	bodyMove.groundSize,	bodyMove.elevation,	delay);*/
		return TRUE;
	}
	else if(bodyMove.behavior == E_BODY_STAR)
	{
		//star position
		/*DrvLegSetTarget(E_LEG_F_L,	DrvLegGetXFromCoxaAngle(15,bodyMove.groundSize)		,bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_M_L,	0													,bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_R_L,	-DrvLegGetXFromCoxaAngle(15,bodyMove.groundSize)	,bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_F_R,	-DrvLegGetXFromCoxaAngle(15,bodyMove.groundSize)	,bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_M_R,	0													,bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_R_R,	DrvLegGetXFromCoxaAngle(15,bodyMove.groundSize)		,bodyMove.groundSize,	bodyMove.elevation,	delay);*/
		return TRUE;
	}
	else if(bodyMove.behavior == E_BODY_STRAIGHT)
	{
		bodyMove.groundSize = 0;
		bodyMove.elevation = 0;
		//straight position
		/*DrvLegSetTarget(E_LEG_F_L,	DrvLegGetXFromCoxaAngle(15,0)	,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_M_L,	0								,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_R_L,	-DrvLegGetXFromCoxaAngle(15,0)	,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_F_R,	-DrvLegGetXFromCoxaAngle(15,0)	,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_M_R,	0								,	bodyMove.groundSize,	bodyMove.elevation,	delay);
		DrvLegSetTarget(E_LEG_R_R,	DrvLegGetXFromCoxaAngle(15,0)	,	bodyMove.groundSize,	bodyMove.elevation,	delay);*/
		return TRUE;
	}
	return FALSE;
}

//Set Ground Size
Boolean SrvBodyMoveSetGroundSize ( float groundSize )
{
	bodyMove.groundSize = groundSize;
	return TRUE;
}
//Get Ground Size
float SrvBodyMoveGetGroundSize ( void )
{
	return bodyMove.groundSize;
}
//Set Elevation
Boolean SrvBodyMoveSetElevation ( float elevation )
{
	bodyMove.elevation = elevation;
	return TRUE;
}
//Set Relative Elevation
Boolean SrvBodyMoveSetRelativeElevation ( float elevation )
{
	bodyMove.elevation += elevation;
	return TRUE;
}
//Get Elevation
float SrvBodyMoveGetElevation ( void )
{
	return bodyMove.elevation;
}