/*
 * DrvLeg.c
 *
 * Created: 18/02/2018 21:58:46
 *  Author: berryer
 */ 
////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf\ConfHard.h"
#include "DrvTick.h"
#include "DrvTimer.h"
#include "DrvLeg.h"


////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////


////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
static SLeg legs [NB_LEGS];
volatile Int8U update = 0U;
uint32_t prevMillisServosUpdate = 0U;

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
static Boolean DrvLegSetOffset ( void ) ;
/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
 static volatile float L, L1;
 static volatile float alpha, alpha1,alpha2,beta,gama;

// Init of Drv PWM
Boolean DrvLegInit( void )
{
	DrvServoInit();
	//init each servos of each leg
	for(Int8U i = 0U ; i < NB_LEGS ; i++ )
	{
		legs[i].id					= i ;
		
		legs[i].coxaAngle				= DrvServoGetStruture(i * 3) ;
		legs[i].coxaAngle->id			= (i * 3);
		legs[i].femurAngle				= DrvServoGetStruture((i * 3) + 1) ;
		legs[i].femurAngle->id			= (i * 3) + 1 ;
		legs[i].tibiaAngle				= DrvServoGetStruture((i * 3) + 2) ;
		legs[i].tibiaAngle->id			= (i * 3) + 2 ;
	}
	//offset and limits
	DrvLegSetOffset();	
	update = 0;
	
	return TRUE;
}

SLeg* DrvLegGetLegs( void )
{
	return &legs;
}

SLeg* DrvLegGetLeg( E_LEG indexLeg )
{
	return &legs[ indexLeg ];
}

Boolean DrvLegCheckTarget( E_LEG indexLeg )
{
	Int8U indexEpaule = 0U + (indexLeg * 3U);
	Int8U indexCoude  = 1U + (indexLeg * 3U);
	Int8U indexPoigne = 2U + (indexLeg * 3U);
	if( 
		DrvServoCheckPosition(indexEpaule) &&
		DrvServoCheckPosition(indexCoude) &&
		DrvServoCheckPosition(indexPoigne) 
		)
		{
			return TRUE;
		}
	return FALSE;
}

Boolean DrvLegSetXYZ( E_LEG indexLeg, Int16S x, Int16S y, Int16S z, Int16U speed )
{	
	//http://te.unib.ac.id/lecturer/indraagustian/2014/05/3dof-inverse-kinematic-for-armleg-of-robot-use-arduino/
	if(indexLeg < E_NB_LEGS)
	{
		//index of servos
		Int8U indexHanche = 0U + (indexLeg * 3U);
		Int8U indexGenoux  = 1U + (indexLeg * 3U);
		Int8U indexCheville = 2U + (indexLeg * 3U);
	
		//add size of members
		y += (LEG_COCYX_LENGHT + LEG_FEMUR_LENGHT );
		z += LEG_TIBIA_LENGHT;
		
		//compute angles from distances
		L1 = (float)sqrt((x * x) + (y * y));
		gama = (float)atan((float)x / (float)y);
		L = (float)sqrt(((L1 - LEG_COCYX_LENGHT) * (L1 - LEG_COCYX_LENGHT)) + (z * z));
		beta = (float)acos(((LEG_TIBIA_LENGHT * LEG_TIBIA_LENGHT) + (LEG_FEMUR_LENGHT * LEG_FEMUR_LENGHT) - (L * L)) / (2 * LEG_TIBIA_LENGHT * LEG_FEMUR_LENGHT));
		alpha1 = (float)acos(z / L);
		alpha2 = (float)acos(((LEG_FEMUR_LENGHT * LEG_FEMUR_LENGHT) + (L * L) - (LEG_TIBIA_LENGHT * LEG_TIBIA_LENGHT)) / (2 * LEG_FEMUR_LENGHT * L));
		alpha = alpha1 + alpha2;
	
		// if left
		if(indexLeg < E_LEG_U_R)
		{
			gama  = (gama * 180) / M_PI;
			alpha = ((-alpha * 180) / M_PI) + 90.0;
			beta  = ((beta * 180) / M_PI) - 90.0;
		}
		//else right
		else
		{
			gama  = (-gama * 180) / M_PI;
			alpha = ((alpha * 180) / M_PI) - 90.0;
			beta  = ((-beta * 180) / M_PI) + 90.0;
		}
	
		//set servos position
		DrvServoSetTarget(indexHanche,		(Int16S)round(gama)		+ legs[ indexLeg ].coxaAngle->mid	,	speed );
		DrvServoSetTarget(indexGenoux,		(Int16S)(round(alpha))	+ legs[ indexLeg ].femurAngle->mid	,	speed );
		DrvServoSetTarget(indexCheville,	(Int16S)(round(beta) )	+ legs[ indexLeg ].tibiaAngle->mid	,	speed );
	
		return TRUE;
	}
	return FALSE;
}

Boolean DrvLegSetPosition( E_LEG indexLeg, Int16S coxaAngle, Int16S femurAngle, Int16S tibiaAngle, Int16U speed )
{
	if(indexLeg < E_NB_LEGS)
	{
		//index of servos
		Int8U indexHanche = 0U + (indexLeg * 3U);
		Int8U indexGenoux  = 1U + (indexLeg * 3U);
		Int8U indexCheville = 2U + (indexLeg * 3U);
		//set servos position
		DrvServoSetTarget(indexHanche, coxaAngle, speed );
		DrvServoSetTarget(indexGenoux, femurAngle,	speed );
		DrvServoSetTarget(indexCheville, tibiaAngle, speed );
		return TRUE;
	}
	return FALSE;
}

Boolean DrvLegUpdate( void )
{
	//update servos every 15 ms
	//if ((DrvTickGetTimeMs() - prevMillisServosUpdate) > 1U)
	{
		//update each servo
		//DrvServoUpdate();
		//prevMillisServosUpdate = DrvTickGetTimeMs();
	}
	DrvServoUpdate();
	return TRUE;
} 



/**
 * \brief set offset and limits to each servo
 * 
 * \param None
 * 
 * \return Boolean if success
 */
static Boolean DrvLegSetOffset ( void ) 
{		
	legs[ E_LEG_U_L ].coxaAngle->currentPosition		= LEG_COXA_U_L_MID ;
	legs[ E_LEG_U_L ].coxaAngle->targetPosition		= LEG_COXA_U_L_MID ;
	legs[ E_LEG_U_L ].coxaAngle->mid			= LEG_COXA_U_L_MID ;
	legs[ E_LEG_U_L ].coxaAngle->min			= LEG_COXA_U_L_MIN ;
	legs[ E_LEG_U_L ].coxaAngle->max			= LEG_COXA_U_L_MAX ;
	legs[ E_LEG_U_L ].femurAngle->currentPosition			= LEG_FEMUR_U_L_MIN ;
	legs[ E_LEG_U_L ].femurAngle->targetPosition		= LEG_FEMUR_U_L_MIN ;
	legs[ E_LEG_U_L ].femurAngle->mid			= LEG_FEMUR_U_L_MID ;
	legs[ E_LEG_U_L ].femurAngle->min			= LEG_FEMUR_U_L_MIN ;
	legs[ E_LEG_U_L ].femurAngle->max			= LEG_FEMUR_U_L_MAX ;
	legs[ E_LEG_U_L ].tibiaAngle->currentPosition		= LEG_TIBIA_U_L_MAX ;
	legs[ E_LEG_U_L ].tibiaAngle->targetPosition		= LEG_TIBIA_U_L_MAX ;
	legs[ E_LEG_U_L ].tibiaAngle->mid			= LEG_TIBIA_U_L_MID ;
	legs[ E_LEG_U_L ].tibiaAngle->min			= LEG_TIBIA_U_L_MIN ;
	legs[ E_LEG_U_L ].tibiaAngle->max			= LEG_TIBIA_U_L_MAX ;
		
	legs[ E_LEG_M_L ].coxaAngle->currentPosition		= LEG_COXA_M_L_MID ;
	legs[ E_LEG_M_L ].coxaAngle->targetPosition		= LEG_COXA_M_L_MID ;
	legs[ E_LEG_M_L ].coxaAngle->mid			= LEG_COXA_M_L_MID ;
	legs[ E_LEG_M_L ].coxaAngle->min			= LEG_COXA_M_L_MIN ;
	legs[ E_LEG_M_L ].coxaAngle->max			= LEG_COXA_M_L_MAX ;
	legs[ E_LEG_M_L ].femurAngle->currentPosition		= LEG_FEMUR_M_L_MAX ;
	legs[ E_LEG_M_L ].femurAngle->targetPosition		= LEG_FEMUR_M_L_MAX ;
	legs[ E_LEG_M_L ].femurAngle->mid			= LEG_FEMUR_M_L_MID ;
	legs[ E_LEG_M_L ].femurAngle->min			= LEG_FEMUR_M_L_MIN ;
	legs[ E_LEG_M_L ].femurAngle->max			= LEG_FEMUR_M_L_MAX ;
	legs[ E_LEG_M_L ].tibiaAngle->currentPosition		= LEG_TIBIA_M_L_MAX ;
	legs[ E_LEG_M_L ].tibiaAngle->targetPosition		= LEG_TIBIA_M_L_MAX ;
	legs[ E_LEG_M_L ].tibiaAngle->mid			= LEG_TIBIA_M_L_MID ;
	legs[ E_LEG_M_L ].tibiaAngle->min			= LEG_TIBIA_M_L_MIN ;
	legs[ E_LEG_M_L ].tibiaAngle->max			= LEG_TIBIA_M_L_MAX ;
		
	legs[ E_LEG_B_L ].coxaAngle->currentPosition		= LEG_COXA_B_L_MID ;
	legs[ E_LEG_B_L ].coxaAngle->targetPosition		= LEG_COXA_B_L_MID ;
	legs[ E_LEG_B_L ].coxaAngle->mid			= LEG_COXA_B_L_MID ;
	legs[ E_LEG_B_L ].coxaAngle->min			= LEG_COXA_B_L_MIN ;
	legs[ E_LEG_B_L ].coxaAngle->max			= LEG_COXA_B_L_MAX ;
	legs[ E_LEG_B_L ].femurAngle->currentPosition		= LEG_FEMUR_B_L_MAX ;
	legs[ E_LEG_B_L ].femurAngle->targetPosition		= LEG_FEMUR_B_L_MAX ;
	legs[ E_LEG_B_L ].femurAngle->mid			= LEG_FEMUR_B_L_MID ;
	legs[ E_LEG_B_L ].femurAngle->min			= LEG_FEMUR_B_L_MIN ;
	legs[ E_LEG_B_L ].femurAngle->max			= LEG_FEMUR_B_L_MAX ;
	legs[ E_LEG_B_L ].tibiaAngle->currentPosition		= LEG_TIBIA_B_L_MAX ;
	legs[ E_LEG_B_L ].tibiaAngle->targetPosition		= LEG_TIBIA_B_L_MAX ;
	legs[ E_LEG_B_L ].tibiaAngle->mid			= LEG_TIBIA_B_L_MID ;
	legs[ E_LEG_B_L ].tibiaAngle->min			= LEG_TIBIA_B_L_MIN ;
	legs[ E_LEG_B_L ].tibiaAngle->max			= LEG_TIBIA_B_L_MAX ;
		
	legs[ E_LEG_U_R ].coxaAngle->currentPosition		= LEG_COXA_U_R_MID ;
	legs[ E_LEG_U_R ].coxaAngle->targetPosition		= LEG_COXA_U_R_MID ;
	legs[ E_LEG_U_R ].coxaAngle->mid			= LEG_COXA_U_R_MID ;
	legs[ E_LEG_U_R ].coxaAngle->min			= LEG_COXA_U_R_MIN ;
	legs[ E_LEG_U_R ].coxaAngle->max			= LEG_COXA_U_R_MAX ;
	legs[ E_LEG_U_R ].femurAngle->currentPosition			= LEG_FEMUR_U_R_MIN ;
	legs[ E_LEG_U_R ].femurAngle->targetPosition		= LEG_FEMUR_U_R_MIN ;
	legs[ E_LEG_U_R ].femurAngle->mid			= LEG_FEMUR_U_R_MID ;
	legs[ E_LEG_U_R ].femurAngle->min			= LEG_FEMUR_U_R_MIN ;
	legs[ E_LEG_U_R ].femurAngle->max			= LEG_FEMUR_U_R_MAX ;
	legs[ E_LEG_U_R ].tibiaAngle->currentPosition		= LEG_TIBIA_U_R_MIN ;
	legs[ E_LEG_U_R ].tibiaAngle->targetPosition		= LEG_TIBIA_U_R_MIN ;
	legs[ E_LEG_U_R ].tibiaAngle->mid			= LEG_TIBIA_U_R_MID ;
	legs[ E_LEG_U_R ].tibiaAngle->min			= LEG_TIBIA_U_R_MIN ;
	legs[ E_LEG_U_R ].tibiaAngle->max			= LEG_TIBIA_U_R_MAX ;
				  
	legs[ E_LEG_M_R ].coxaAngle->currentPosition		= LEG_COXA_M_R_MID ;
	legs[ E_LEG_M_R ].coxaAngle->targetPosition		= LEG_COXA_M_R_MID ;
	legs[ E_LEG_M_R ].coxaAngle->mid			= LEG_COXA_M_R_MID ;
	legs[ E_LEG_M_R ].coxaAngle->min			= LEG_COXA_M_R_MIN ;
	legs[ E_LEG_M_R ].coxaAngle->max			= LEG_COXA_M_R_MAX ;
	legs[ E_LEG_M_R ].femurAngle->currentPosition		= LEG_FEMUR_M_R_MIN ;
	legs[ E_LEG_M_R ].femurAngle->targetPosition		= LEG_FEMUR_M_R_MIN ;
	legs[ E_LEG_M_R ].femurAngle->mid			= LEG_FEMUR_M_R_MID ;
	legs[ E_LEG_M_R ].femurAngle->min			= LEG_FEMUR_M_R_MIN ;
	legs[ E_LEG_M_R ].femurAngle->max			= LEG_FEMUR_M_R_MAX ;
	legs[ E_LEG_M_R ].tibiaAngle->currentPosition		= LEG_TIBIA_M_R_MIN ;
	legs[ E_LEG_M_R ].tibiaAngle->targetPosition		= LEG_TIBIA_M_R_MIN ;
	legs[ E_LEG_M_R ].tibiaAngle->mid			= LEG_TIBIA_M_R_MID ;
	legs[ E_LEG_M_R ].tibiaAngle->min			= LEG_TIBIA_M_R_MIN ;
	legs[ E_LEG_M_R ].tibiaAngle->max			= LEG_TIBIA_M_R_MAX ;
				  
	legs[ E_LEG_B_R ].coxaAngle->currentPosition		= LEG_COXA_B_R_MID ;
	legs[ E_LEG_B_R ].coxaAngle->targetPosition		= LEG_COXA_B_R_MID ;
	legs[ E_LEG_B_R ].coxaAngle->mid			= LEG_COXA_B_R_MID ;
	legs[ E_LEG_B_R ].coxaAngle->min			= LEG_COXA_B_R_MIN ;
	legs[ E_LEG_B_R ].coxaAngle->max			= LEG_COXA_B_R_MAX ;
	legs[ E_LEG_B_R ].femurAngle->currentPosition		= LEG_FEMUR_B_R_MIN ;
	legs[ E_LEG_B_R ].femurAngle->targetPosition		= LEG_FEMUR_B_R_MIN ;
	legs[ E_LEG_B_R ].femurAngle->mid			= LEG_FEMUR_B_R_MID ;
	legs[ E_LEG_B_R ].femurAngle->min			= LEG_FEMUR_B_R_MIN ;
	legs[ E_LEG_B_R ].femurAngle->max			= LEG_FEMUR_B_R_MAX ;
	legs[ E_LEG_B_R ].tibiaAngle->currentPosition		= LEG_TIBIA_B_R_MIN ;
	legs[ E_LEG_B_R ].tibiaAngle->targetPosition		= LEG_TIBIA_B_R_MIN ;
	legs[ E_LEG_B_R ].tibiaAngle->mid			= LEG_TIBIA_B_R_MID ;
	legs[ E_LEG_B_R ].tibiaAngle->min			= LEG_TIBIA_B_R_MIN ;
	legs[ E_LEG_B_R ].tibiaAngle->max			= LEG_TIBIA_B_R_MAX ;
		
	return TRUE;
}