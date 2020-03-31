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
#define SERVO_REFRESH_PERIOD	10U
#define SERVO_INITIALIZE_STEP	50U

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
static SLegs legs;

//period of refresh
uint32_t prevMillisServosUpdate = 0U;

//permits to senable servo step by step
uint8_t servoIndex = 0U;
uint16_t servosInitializingPeriod = 0U;

//variable for trigonometry
static volatile float L, L1;
static volatile float alpha, alpha1,alpha2,beta,gama;

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
//permits to set offset of bot
static Boolean DrvLegSetOffset ( void ) ;
/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////

// Init of Drv PWM
Boolean DrvLegInit( void )
{
	//permits to enable servo step by step
	servoIndex = 0U;
	legs.isInitializing = TRUE;
	servosInitializingPeriod = 0U;
	
	//initialize the driver
	DrvServoInit();
	
	//get each servos for each leg and set member id
	for(Int8U i = 0U ; i < NB_LEGS ; i++ )
	{
		legs.leg[i].id				= i ;
		legs.leg[i].coxaAngle		= DrvServoGetStruture(i * 3) ;
		legs.leg[i].coxaAngle->id	= (i * 3);
		legs.leg[i].femurAngle		= DrvServoGetStruture((i * 3) + 1) ;
		legs.leg[i].femurAngle->id	= (i * 3) + 1 ;
		legs.leg[i].tibiaAngle		= DrvServoGetStruture((i * 3) + 2) ;
		legs.leg[i].tibiaAngle->id	= (i * 3) + 2 ;
	}
	
	//offset and limits
	DrvLegSetOffset();	
		
	return TRUE;
}

SLegs* DrvLegGetLegs( void )
{
	return &legs;
}

SLeg* DrvLegGetLeg( E_LEG indexLeg )
{
	return &legs.leg[indexLeg];
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

Boolean DrvLegGetXYZ(E_LEG indexLeg, Int16S *x, Int16S *y, Int16S *z )
{
	x[0] = legs.leg[ indexLeg ].x;
	y[0] = legs.leg[ indexLeg ].y;
	z[0] = legs.leg[ indexLeg ].z;
	return TRUE;
}
Boolean DrvLegSetXYZ( E_LEG indexLeg, Int16S x, Int16S y, Int16S z, Int16U speed )
{	
	//http://te.unib.ac.id/lecturer/indraagustian/2014/05/3dof-inverse-kinematic-for-armleg-of-robot-use-arduino/
	if(indexLeg < E_NB_LEGS)
	{
		legs.leg[ indexLeg ].x = x;
		legs.leg[ indexLeg ].y = y;
		legs.leg[ indexLeg ].z = z;
		
		//index of servos
		Int8U indexHanche = 0U + (indexLeg * 3U);
		Int8U indexGenoux  = 1U + (indexLeg * 3U);
		Int8U indexCheville = 2U + (indexLeg * 3U);
	
		//add size of members
		x += 0U;
		y += LEG_COCYX_LENGHT;
		z += (LEG_TIBIA_LENGHT - LEG_FEMUR_LENGHT);
		
		//compute angles from distances
		L1 = (float)sqrt((x * x) + (y * y));
		gama = (float)atan((float)x / (float)y);
		L = (float)sqrt(((L1 - LEG_COCYX_LENGHT) * (L1 - LEG_COCYX_LENGHT)) + (z * z));
		beta = (float)acos(((LEG_TIBIA_LENGHT * LEG_TIBIA_LENGHT) + (LEG_FEMUR_LENGHT * LEG_FEMUR_LENGHT) - (L * L)) / (2 * LEG_TIBIA_LENGHT * LEG_FEMUR_LENGHT));
		alpha1 = (float)acos(z / L);
		alpha2 = (float)acos(((LEG_FEMUR_LENGHT * LEG_FEMUR_LENGHT) + (L * L) - (LEG_TIBIA_LENGHT * LEG_TIBIA_LENGHT)) / (2 * LEG_FEMUR_LENGHT * L));
		alpha = alpha1 + alpha2;
	
		gama  = (gama * 180) / M_PI;
		alpha = (alpha * 180) / M_PI;
		beta  = (beta * 180) / M_PI;
	
		//set servos position
		uint8_t reponse = 0;
		if((indexLeg == E_LEG_U_L)||(indexLeg == E_LEG_M_L)||(indexLeg == E_LEG_B_L))
		{
			alpha = SetRange(alpha,0,180,legs.leg[ indexLeg ].femurAngle->min,legs.leg[ indexLeg ].femurAngle->max);
			beta = SetRange(beta,0,180,legs.leg[ indexLeg ].tibiaAngle->max,legs.leg[ indexLeg ].tibiaAngle->min);
		}
		else
		{
			alpha = SetRange(alpha,0,180,legs.leg[ indexLeg ].femurAngle->max,legs.leg[ indexLeg ].femurAngle->min);
			beta = SetRange(beta,0,180,legs.leg[ indexLeg ].tibiaAngle->min,legs.leg[ indexLeg ].tibiaAngle->max);
		}
		
		if((indexLeg == E_LEG_U_L)||(indexLeg == E_LEG_B_R))
		{
			gama += legs.leg[ indexLeg ].coxaAngle->min;
		}
		else if((indexLeg == E_LEG_B_L)||(indexLeg == E_LEG_U_R))
		{
			gama += legs.leg[ indexLeg ].coxaAngle->max;
		}
		else if((indexLeg == E_LEG_M_L)||(indexLeg == E_LEG_M_R))
		{
			gama += legs.leg[ indexLeg ].coxaAngle->mid;
		}
		
		reponse = DrvServoSetTarget(indexHanche,	(Int16S)round(gama)	,	speed );
		reponse *= 10U;
		reponse += DrvServoSetTarget(indexGenoux,	(Int16S)(round(alpha)),	speed );
		reponse *= 10U;
		reponse += DrvServoSetTarget(indexCheville,	(Int16S)(round(beta) ),	speed );
		
		return reponse == 111U ? TRUE : FALSE;
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

Boolean DrvLegIsInitialized( void )
{
	return !legs.isInitializing ? TRUE : FALSE;
}

Boolean DrvLegUpdate( void )
{
	//if servos are initializing so activate servos step by step with step of 500
	if ((DrvTickGetTimeMs() - prevMillisServosUpdate) > SERVO_REFRESH_PERIOD)
	{
		//if initializing time
		if(legs.isInitializing == TRUE)
		{
			//count up to SERVO_INITIALIZE_STEP
			servosInitializingPeriod++;
			if(servosInitializingPeriod * SERVO_REFRESH_PERIOD > SERVO_INITIALIZE_STEP)
			{
				//reset counter
				servosInitializingPeriod = 0U;
				//enable servo
				DrvServoActivate(servoIndex,TRUE);
				//prepare for next servo
				servoIndex++;
				
				if(servoIndex == NB_SERVOS )
				{
					//initialization done
					legs.isInitializing = FALSE;
				}
			}
		}
		prevMillisServosUpdate = DrvTickGetTimeMs();
	}
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
	legs.leg[ E_LEG_U_L ].coxaAngle->currentPosition		= LEG_COXA_U_L_MIN ;
	legs.leg[ E_LEG_U_L ].coxaAngle->targetPosition		= LEG_COXA_U_L_MIN ;
	legs.leg[ E_LEG_U_L ].coxaAngle->mid			= LEG_COXA_U_L_MID ;
	legs.leg[ E_LEG_U_L ].coxaAngle->min			= LEG_COXA_U_L_MIN ;
	legs.leg[ E_LEG_U_L ].coxaAngle->max			= LEG_COXA_U_L_MAX ;
	legs.leg[ E_LEG_U_L ].femurAngle->currentPosition		= LEG_FEMUR_U_L_MAX ;
	legs.leg[ E_LEG_U_L ].femurAngle->targetPosition		= LEG_FEMUR_U_L_MAX ;
	legs.leg[ E_LEG_U_L ].femurAngle->mid			= LEG_FEMUR_U_L_MID ;
	legs.leg[ E_LEG_U_L ].femurAngle->min			= LEG_FEMUR_U_L_MIN ;
	legs.leg[ E_LEG_U_L ].femurAngle->max			= LEG_FEMUR_U_L_MAX ;
	legs.leg[ E_LEG_U_L ].tibiaAngle->currentPosition		= LEG_TIBIA_U_L_MAX ;
	legs.leg[ E_LEG_U_L ].tibiaAngle->targetPosition		= LEG_TIBIA_U_L_MAX ;
	legs.leg[ E_LEG_U_L ].tibiaAngle->mid			= LEG_TIBIA_U_L_MID ;
	legs.leg[ E_LEG_U_L ].tibiaAngle->min			= LEG_TIBIA_U_L_MIN ;
	legs.leg[ E_LEG_U_L ].tibiaAngle->max			= LEG_TIBIA_U_L_MAX ;
	
	legs.leg[ E_LEG_M_L ].coxaAngle->currentPosition		= LEG_COXA_M_L_MID ;
	legs.leg[ E_LEG_M_L ].coxaAngle->targetPosition		= LEG_COXA_M_L_MID ;
	legs.leg[ E_LEG_M_L ].coxaAngle->mid			= LEG_COXA_M_L_MID ;
	legs.leg[ E_LEG_M_L ].coxaAngle->min			= LEG_COXA_M_L_MIN ;
	legs.leg[ E_LEG_M_L ].coxaAngle->max			= LEG_COXA_M_L_MAX ;
	legs.leg[ E_LEG_M_L ].femurAngle->currentPosition		= LEG_FEMUR_M_L_MAX ;
	legs.leg[ E_LEG_M_L ].femurAngle->targetPosition		= LEG_FEMUR_M_L_MAX ;
	legs.leg[ E_LEG_M_L ].femurAngle->mid			= LEG_FEMUR_M_L_MID ;
	legs.leg[ E_LEG_M_L ].femurAngle->min			= LEG_FEMUR_M_L_MIN ;
	legs.leg[ E_LEG_M_L ].femurAngle->max			= LEG_FEMUR_M_L_MAX ;
	legs.leg[ E_LEG_M_L ].tibiaAngle->currentPosition		= LEG_TIBIA_M_L_MAX ;
	legs.leg[ E_LEG_M_L ].tibiaAngle->targetPosition		= LEG_TIBIA_M_L_MAX ;
	legs.leg[ E_LEG_M_L ].tibiaAngle->mid			= LEG_TIBIA_M_L_MID ;
	legs.leg[ E_LEG_M_L ].tibiaAngle->min			= LEG_TIBIA_M_L_MIN ;
	legs.leg[ E_LEG_M_L ].tibiaAngle->max			= LEG_TIBIA_M_L_MAX ;
	
	legs.leg[ E_LEG_B_L ].coxaAngle->currentPosition		= LEG_COXA_B_L_MAX ;
	legs.leg[ E_LEG_B_L ].coxaAngle->targetPosition		= LEG_COXA_B_L_MAX ;
	legs.leg[ E_LEG_B_L ].coxaAngle->mid			= LEG_COXA_B_L_MID ;
	legs.leg[ E_LEG_B_L ].coxaAngle->min			= LEG_COXA_B_L_MIN ;
	legs.leg[ E_LEG_B_L ].coxaAngle->max			= LEG_COXA_B_L_MAX ;
	legs.leg[ E_LEG_B_L ].femurAngle->currentPosition		= LEG_FEMUR_B_L_MAX ;
	legs.leg[ E_LEG_B_L ].femurAngle->targetPosition		= LEG_FEMUR_B_L_MAX ;
	legs.leg[ E_LEG_B_L ].femurAngle->mid			= LEG_FEMUR_B_L_MID ;
	legs.leg[ E_LEG_B_L ].femurAngle->min			= LEG_FEMUR_B_L_MIN ;
	legs.leg[ E_LEG_B_L ].femurAngle->max			= LEG_FEMUR_B_L_MAX ;
	legs.leg[ E_LEG_B_L ].tibiaAngle->currentPosition		= LEG_TIBIA_B_L_MAX ;
	legs.leg[ E_LEG_B_L ].tibiaAngle->targetPosition		= LEG_TIBIA_B_L_MAX ;
	legs.leg[ E_LEG_B_L ].tibiaAngle->mid			= LEG_TIBIA_B_L_MID ;
	legs.leg[ E_LEG_B_L ].tibiaAngle->min			= LEG_TIBIA_B_L_MIN ;
	legs.leg[ E_LEG_B_L ].tibiaAngle->max			= LEG_TIBIA_B_L_MAX ;
	
	legs.leg[ E_LEG_U_R ].coxaAngle->currentPosition		= LEG_COXA_U_R_MAX ;
	legs.leg[ E_LEG_U_R ].coxaAngle->targetPosition		= LEG_COXA_U_R_MAX ;
	legs.leg[ E_LEG_U_R ].coxaAngle->mid			= LEG_COXA_U_R_MID ;
	legs.leg[ E_LEG_U_R ].coxaAngle->min			= LEG_COXA_U_R_MIN ;
	legs.leg[ E_LEG_U_R ].coxaAngle->max			= LEG_COXA_U_R_MAX ;
	legs.leg[ E_LEG_U_R ].femurAngle->currentPosition		= LEG_FEMUR_U_R_MIN ;
	legs.leg[ E_LEG_U_R ].femurAngle->targetPosition		= LEG_FEMUR_U_R_MIN ;
	legs.leg[ E_LEG_U_R ].femurAngle->mid			= LEG_FEMUR_U_R_MID ;
	legs.leg[ E_LEG_U_R ].femurAngle->min			= LEG_FEMUR_U_R_MIN ;
	legs.leg[ E_LEG_U_R ].femurAngle->max			= LEG_FEMUR_U_R_MAX ;
	legs.leg[ E_LEG_U_R ].tibiaAngle->currentPosition		= LEG_TIBIA_U_R_MIN ;
	legs.leg[ E_LEG_U_R ].tibiaAngle->targetPosition		= LEG_TIBIA_U_R_MIN ;
	legs.leg[ E_LEG_U_R ].tibiaAngle->mid			= LEG_TIBIA_U_R_MID ;
	legs.leg[ E_LEG_U_R ].tibiaAngle->min			= LEG_TIBIA_U_R_MIN ;
	legs.leg[ E_LEG_U_R ].tibiaAngle->max			= LEG_TIBIA_U_R_MAX ;
			  
	legs.leg[ E_LEG_M_R ].coxaAngle->currentPosition		= LEG_COXA_M_R_MID ;
	legs.leg[ E_LEG_M_R ].coxaAngle->targetPosition		= LEG_COXA_M_R_MID ;
	legs.leg[ E_LEG_M_R ].coxaAngle->mid			= LEG_COXA_M_R_MID ;
	legs.leg[ E_LEG_M_R ].coxaAngle->min			= LEG_COXA_M_R_MIN ;
	legs.leg[ E_LEG_M_R ].coxaAngle->max			= LEG_COXA_M_R_MAX ;
	legs.leg[ E_LEG_M_R ].femurAngle->currentPosition		= LEG_FEMUR_M_R_MIN ;
	legs.leg[ E_LEG_M_R ].femurAngle->targetPosition		= LEG_FEMUR_M_R_MIN ;
	legs.leg[ E_LEG_M_R ].femurAngle->mid			= LEG_FEMUR_M_R_MID ;
	legs.leg[ E_LEG_M_R ].femurAngle->min			= LEG_FEMUR_M_R_MIN ;
	legs.leg[ E_LEG_M_R ].femurAngle->max			= LEG_FEMUR_M_R_MAX ;
	legs.leg[ E_LEG_M_R ].tibiaAngle->currentPosition		= LEG_TIBIA_M_R_MIN ;
	legs.leg[ E_LEG_M_R ].tibiaAngle->targetPosition		= LEG_TIBIA_M_R_MIN ;
	legs.leg[ E_LEG_M_R ].tibiaAngle->mid			= LEG_TIBIA_M_R_MID ;
	legs.leg[ E_LEG_M_R ].tibiaAngle->min			= LEG_TIBIA_M_R_MIN ;
	legs.leg[ E_LEG_M_R ].tibiaAngle->max			= LEG_TIBIA_M_R_MAX ;
			  
	legs.leg[ E_LEG_B_R ].coxaAngle->currentPosition		= LEG_COXA_B_R_MIN ;
	legs.leg[ E_LEG_B_R ].coxaAngle->targetPosition		= LEG_COXA_B_R_MIN ;
	legs.leg[ E_LEG_B_R ].coxaAngle->mid			= LEG_COXA_B_R_MID ;
	legs.leg[ E_LEG_B_R ].coxaAngle->min			= LEG_COXA_B_R_MIN ;
	legs.leg[ E_LEG_B_R ].coxaAngle->max			= LEG_COXA_B_R_MAX ;
	legs.leg[ E_LEG_B_R ].femurAngle->currentPosition		= LEG_FEMUR_B_R_MIN ;
	legs.leg[ E_LEG_B_R ].femurAngle->targetPosition		= LEG_FEMUR_B_R_MIN ;
	legs.leg[ E_LEG_B_R ].femurAngle->mid			= LEG_FEMUR_B_R_MID ;
	legs.leg[ E_LEG_B_R ].femurAngle->min			= LEG_FEMUR_B_R_MIN ;
	legs.leg[ E_LEG_B_R ].femurAngle->max			= LEG_FEMUR_B_R_MAX ;
	legs.leg[ E_LEG_B_R ].tibiaAngle->currentPosition		= LEG_TIBIA_B_R_MIN ;
	legs.leg[ E_LEG_B_R ].tibiaAngle->targetPosition		= LEG_TIBIA_B_R_MIN ;
	legs.leg[ E_LEG_B_R ].tibiaAngle->mid			= LEG_TIBIA_B_R_MID ;
	legs.leg[ E_LEG_B_R ].tibiaAngle->min			= LEG_TIBIA_B_R_MIN ;
	legs.leg[ E_LEG_B_R ].tibiaAngle->max			= LEG_TIBIA_B_R_MAX ;
		
	return TRUE;
}