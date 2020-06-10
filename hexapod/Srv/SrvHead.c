/*
 * SrvHead.c
 *
 * Created: 03/06/2020 14:32:27
 *  Author: louis
 */ 
////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf\ConfHard.h"
#include "SrvHead.h"

#include "Drv/DrvTick.h"
#include "Drv/DrvLeg.h"


////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define HEAD_HORI_SERO_TAB_INDEX	(E_NB_LEGS * NB_SERVOS_PER_LEG) + HEAD_HORI_SERVO_INDEX

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
static SHead head;


////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
void SrvHeadSetMidPosition( Int16U time );
void SrvHeadSetMaxPosition( Int16U time );
void SrvHeadSetMinPosition( Int16U time );
void SrvHeadServoCallbackReachMin (Int8U index);
void SrvHeadServoCallbackReachMid (Int8U index);
void SrvHeadServoCallbackReachMax (Int8U index);
/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////

// Init of Drv Head
Boolean SrvHeadInit( void )
{	
	head.enable			= TRUE ;
	head.initialized	= FALSE ;
	head.scanning		= FALSE ;
	
	head.servos[ HEAD_HORI_SERVO_INDEX ] = DrvServoGetStruture( HEAD_HORI_SERO_TAB_INDEX ) ;
	head.servos[ HEAD_HORI_SERVO_INDEX ]->id					= 18 ;
	head.servos[ HEAD_HORI_SERVO_INDEX ]->currentPosition		= HEAD_HORI_MID ;
	head.servos[ HEAD_HORI_SERVO_INDEX ]->targetPosition		= HEAD_HORI_MID ;
	head.servos[ HEAD_HORI_SERVO_INDEX ]->offset				= HEAD_HORI_OFT ;
	head.servos[ HEAD_HORI_SERVO_INDEX ]->mid					= HEAD_HORI_MID ;
	head.servos[ HEAD_HORI_SERVO_INDEX ]->min					= HEAD_HORI_MIN ;
	head.servos[ HEAD_HORI_SERVO_INDEX ]->max					= HEAD_HORI_MAX ;
	head.servos[ HEAD_HORI_SERVO_INDEX ]->enable				= TRUE ;

	head.initialized	= TRUE ;
	return TRUE;
}


void SrvHeadUpdate( void )
{
	
}

Boolean SrvHeadScan( void )
{
	if(head.enable)
	{
		head.scanning = TRUE ;
		
		SrvHeadSetMinPosition(1000);
		DrvServoSetCallback(HEAD_HORI_SERO_TAB_INDEX, SrvHeadServoCallbackReachMin);
		return TRUE;
	}
	return FALSE;
}

SHead* SrvHeadGetStruct( void )
{
	return &head;
}

void SrvHeadSetNotification (SrvHeadNotification cb) 
{
	 head.notification = cb;
}

/////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
void SrvHeadSetMidPosition( Int16U time )
{
	if(head.enable) DrvServoSetTarget(HEAD_HORI_SERO_TAB_INDEX, HEAD_HORI_MID, time);
}

void SrvHeadSetMaxPosition( Int16U time )
{
	if(head.enable) DrvServoSetTarget(HEAD_HORI_SERO_TAB_INDEX, HEAD_HORI_MAX, time);
}

void SrvHeadSetMinPosition( Int16U time )
{
	if(head.enable) DrvServoSetTarget(HEAD_HORI_SERO_TAB_INDEX, HEAD_HORI_MIN, time);
}

void SrvHeadServoCallbackReachMin (Int8U index)
{
	SrvHeadSetMaxPosition(2000);
	DrvServoSetCallback(HEAD_HORI_SERO_TAB_INDEX, SrvHeadServoCallbackReachMax);
}

void SrvHeadServoCallbackReachMax (Int8U index)
{
	SrvHeadSetMidPosition(1000);
	DrvServoSetCallback(HEAD_HORI_SERO_TAB_INDEX, SrvHeadServoCallbackReachMid);
}

void SrvHeadServoCallbackReachMid (Int8U index)
{
	head.scanning = FALSE ;
	DrvServoSetCallback(HEAD_HORI_SERO_TAB_INDEX, NULL);
}



