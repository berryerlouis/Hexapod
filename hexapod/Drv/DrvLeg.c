/*
 * DrvLeg.c
 *
 * Created: 18/02/2018 21:58:46
 *  Author: berryer
 */ 
////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf\ConfHard.h"
#include "DrvTick.h"
#include "DrvLeg.h"


////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define SERVO_INTIALIZATION_PERIOD	250U

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
static SLegs legs;

//period of refresh
uint32_t prevMillisServosInitialize = 0U;

uint8_t legIndexInitialization = 0U;


////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
//permits to set offset of bot
static Boolean DrvLegSetOffset ( void ) ;
static void DrvLegInititalizing( void );
static Boolean DrvLegApplyPosition( ELeg indexLeg, float coxaAngle, float femurAngle, float tibiaAngle, Int16U speed );
static Boolean DrvLegComputeInverseKinematics( AxisFloat position, float *coxaAngle, float *femurAngle, float *tibiaAngle );
/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////

// Init of Drv PWM
Boolean DrvLegInit( void )
{
	DrvServoInit();
	//permits to enable servo step by step
	legIndexInitialization = 0U;
	legs.isInitialized = FALSE;
		
	//get each servos for each leg and set member id
	for(Int8U indexLeg = 0U ; indexLeg < E_NB_LEGS ; indexLeg++ )
	{
		legs.leg[ indexLeg ].id				= indexLeg ;
		legs.leg[ indexLeg ].enable			= FALSE ;
		legs.leg[ indexLeg ].initialized		= FALSE ;
		legs.leg[ indexLeg ].newPosition		= FALSE ;
		legs.leg[ indexLeg ].coxaAngle		= DrvServoGetStruture(indexLeg * NB_SERVOS_PER_LEG) ;
		legs.leg[ indexLeg ].coxaAngle->id	= (indexLeg * NB_SERVOS_PER_LEG);
		legs.leg[ indexLeg ].femurAngle		= DrvServoGetStruture((indexLeg * NB_SERVOS_PER_LEG) + 1) ;
		legs.leg[ indexLeg ].femurAngle->id	= (indexLeg * NB_SERVOS_PER_LEG) + 1 ;
		legs.leg[ indexLeg ].tibiaAngle		= DrvServoGetStruture((indexLeg * NB_SERVOS_PER_LEG) + 2) ;
		legs.leg[ indexLeg ].tibiaAngle->id	= (indexLeg * NB_SERVOS_PER_LEG) + 2 ;
	}
	
	//offset and limits
	DrvLegSetOffset();	
		
	return TRUE;
}

SLegs* DrvLegGetLegs( void )
{
	return &legs;
}

SLeg* DrvLegGetStruct( ELeg indexLeg )
{
	return &legs.leg[indexLeg];
}

Boolean DrvLegSetTarget( ELeg indexLeg, AxisFloat position, Int16U speed  )
{
	if(DrvLegReachPosition(indexLeg))
	{
		//set the next targets
		legs.leg[ indexLeg ].targetPosition.x = position.x;  
		legs.leg[ indexLeg ].targetPosition.y = position.y;
		legs.leg[ indexLeg ].targetPosition.z = position.z; 
		legs.leg[ indexLeg ].startTime = DrvTickGetTimeMs();
		legs.leg[ indexLeg ].speed = speed;
		legs.leg[ indexLeg ].newPosition = TRUE;
		return TRUE;
	}
	return FALSE;
}

Boolean DrvLegSetSpeed( ELeg indexLeg, Int16U speed  )
{
	legs.leg[ indexLeg ].speed = speed;
	return TRUE;
}

AxisFloat DrvLegGetXYZ(ELeg indexLeg )
{
	return legs.leg[ indexLeg ].targetPosition;
}

Boolean DrvLegReachPosition( ELeg indexLeg )
{
	return !legs.leg[ indexLeg ].newPosition;
}

Boolean DrvLegIsInitialized( void )
{
	return legs.isInitialized;
}

Int32U prevMillisLegUpdate = 0U;
Boolean DrvLegUpdate( void )
{
	//if servos are initializing 
	if (DrvLegIsInitialized() == FALSE)
	{
		//activating servos step by step
		DrvLegInititalizing();
	}
	else
	{
		for(ELeg indexLeg = 0U ; indexLeg < E_NB_LEGS ; indexLeg++ )
		{
			if(legs.leg[ indexLeg ].newPosition)
			{
				AxisFloat position;
				float percentage = (DrvTickGetTimeMs() - legs.leg[ indexLeg ].startTime) / (float)legs.leg[ indexLeg ].speed;
				position = LerpAxis(legs.leg[ indexLeg ].startPosition,legs.leg[ indexLeg ].targetPosition,percentage);
				
				float coxaAngle  = 0;
				float femurAngle = 0;
				float tibiaAngle = 0;
				DrvLegComputeInverseKinematics(
					position,
					&coxaAngle,
					&femurAngle,
					&tibiaAngle);
				//apply new position
				DrvLegApplyPosition(indexLeg,
					coxaAngle,
					femurAngle ,
					tibiaAngle,
					legs.leg[ indexLeg ].speed);
				
				if(percentage >= 1.0)
				{
					legs.leg[ indexLeg ].newPosition = FALSE;
					legs.leg[ indexLeg ].startPosition.x = legs.leg[ indexLeg ].targetPosition.x;
					legs.leg[ indexLeg ].startPosition.y = legs.leg[ indexLeg ].targetPosition.y;
					legs.leg[ indexLeg ].startPosition.z = legs.leg[ indexLeg ].targetPosition.z;
				}
			}
		}
	}
	//update each servo
	DrvServoUpdate();
	return TRUE;
}

float DrvLegGetXFromCoxaAngle( float angle , float y)
{
	return (y + LEG_COCYX_LENGTH) * tan(ToRad(angle));
}


////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
static Boolean DrvLegComputeInverseKinematics(AxisFloat position, float *coxaAngle, float *femurAngle, float *tibiaAngle )
{
	//http://te.unib.ac.id/lecturer/indraagustian/2014/05/3dof-inverse-kinematic-for-armleg-of-robot-use-arduino/
	//variable for trigonometry
	float L, L1;
	float alpha, alpha1,alpha2,beta,gama;
	//add size of members
	position.x += 0;
	position.y += LEG_COCYX_LENGTH;
	position.z += (LEG_TIBIA_LENGTH - LEG_FEMUR_LENGTH);
		
	//compute angles from distances
	L1 = (float)sqrt((float)((float)(position.x * position.x) + (float)(position.y * position.y)));
	gama = (float)atan((float)position.x / (float)position.y);
	L = (float)sqrt(((L1 - LEG_COCYX_LENGTH) * (L1 - LEG_COCYX_LENGTH)) + (float)(position.z * position.z));
	beta = (float)acos(((LEG_TIBIA_LENGTH * LEG_TIBIA_LENGTH) + (LEG_FEMUR_LENGTH * LEG_FEMUR_LENGTH) - (L * L)) / (2 * LEG_TIBIA_LENGTH * LEG_FEMUR_LENGTH));
	alpha1 = (float)acos((float)position.z / L);
	alpha2 = (float)acos(((LEG_FEMUR_LENGTH * LEG_FEMUR_LENGTH) + (L * L) - (LEG_TIBIA_LENGTH * LEG_TIBIA_LENGTH)) / (2 * LEG_FEMUR_LENGTH * L));
	alpha = alpha1 + alpha2;
		
	gama  = (gama * 180) / M_PI;
	alpha = (alpha * 180) / M_PI;
	beta  = (beta * 180) / M_PI;
	
	coxaAngle[0U] = gama * 10;
	femurAngle[0U] = alpha * 10;
	tibiaAngle[0U] = beta * 10;
	return TRUE;
}

static Boolean DrvLegApplyPosition( ELeg indexLeg, float coxaAngle, float femurAngle, float tibiaAngle, Int16U speed )
{	
	if(
		(indexLeg < E_NB_LEGS) &&
		(legs.leg[indexLeg].initialized == TRUE )&&
		(legs.leg[indexLeg].enable == TRUE )
	)
	{	
		//index of servos
		Int8U indexHanche = 0U + (indexLeg * NB_SERVOS_PER_LEG);
		Int8U indexGenoux  = 1U + (indexLeg * NB_SERVOS_PER_LEG);
		Int8U indexCheville = 2U + (indexLeg * NB_SERVOS_PER_LEG);
		Int16S coxaAngleTemp = (Int16S)round(coxaAngle);
		Int16S femurAngleTemp = (Int16S)round(femurAngle);
		Int16S tibiaAngleTemp = (Int16S)round(tibiaAngle);
		
		if((indexLeg == E_LEG_F_L)||(indexLeg == E_LEG_M_L)||(indexLeg == E_LEG_R_L))
		{
			//limits and invert beta
			femurAngleTemp = femurAngleTemp - SERVO_ANGLE_MAX + legs.leg[ indexLeg ].femurAngle->max;
			tibiaAngleTemp = -1 * tibiaAngleTemp + legs.leg[ indexLeg ].tibiaAngle->max;
		}
		//right side
		else
		{
			//limits and invert beta
			femurAngleTemp = -femurAngleTemp + SERVO_ANGLE_MAX + legs.leg[ indexLeg ].femurAngle->min;
			tibiaAngleTemp = tibiaAngleTemp + legs.leg[ indexLeg ].tibiaAngle->min;
		}
				
		//same for the gama angle
		if((indexLeg == E_LEG_F_L)||(indexLeg == E_LEG_R_R))
		{
			coxaAngleTemp += legs.leg[ indexLeg ].coxaAngle->min;
		}
		else if((indexLeg == E_LEG_M_L)||(indexLeg == E_LEG_M_R))
		{
			coxaAngleTemp += legs.leg[ indexLeg ].coxaAngle->min;
		}
		else if((indexLeg == E_LEG_R_L)||(indexLeg == E_LEG_F_R))
		{
			coxaAngleTemp += legs.leg[ indexLeg ].coxaAngle->min;
		}
		
		Int8U reponse = 0;
		//send to servo
		reponse = DrvServoSetTarget(indexHanche,	coxaAngleTemp,	speed );
		reponse *= 10U;
		reponse += DrvServoSetTarget(indexGenoux,	femurAngleTemp,speed );
		reponse *= 10U;
		reponse += DrvServoSetTarget(indexCheville,	tibiaAngleTemp,speed );
			
		if(reponse == 111U)
		{
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

static void DrvLegInititalizing( void )
{
	//if initializing time
	if((DrvTickGetTimeMs() - prevMillisServosInitialize) > SERVO_INTIALIZATION_PERIOD)
	{
		if(legIndexInitialization < E_NB_LEGS )
		{
			AxisFloat position = {.x=0,.y=0,.z=0};
			float coxaAngle  = 0;
			float femurAngle = 0;
			float tibiaAngle = 0;
			DrvLegComputeInverseKinematics(
				position,
				&coxaAngle,
				&femurAngle,
				&tibiaAngle);
			//apply new position
			DrvLegApplyPosition(legIndexInitialization,
				coxaAngle,
				femurAngle ,
				tibiaAngle,
				1000);
			//enable servo
			DrvServoActivate((legIndexInitialization * NB_SERVOS_PER_LEG) + 0,TRUE);
			DrvServoActivate((legIndexInitialization * NB_SERVOS_PER_LEG) + 1,TRUE);
			DrvServoActivate((legIndexInitialization * NB_SERVOS_PER_LEG) + 2,TRUE);
			//leg is now enable
			legs.leg[legIndexInitialization].enable = TRUE ;
			//leg is now initialized
			legs.leg[legIndexInitialization].initialized = TRUE ;
		}
		else
		{
			//initialization done
			legs.isInitialized = TRUE;
		}
		//prepare for next servo
		legIndexInitialization++;
		prevMillisServosInitialize = DrvTickGetTimeMs();
	}
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
	legs.leg[ E_LEG_F_L ].coxaAngle->currentPosition		= LEG_COCYX_U_L_OFT ;
	legs.leg[ E_LEG_F_L ].coxaAngle->offset					= LEG_COCYX_U_L_OFT ;
	legs.leg[ E_LEG_F_L ].coxaAngle->mid					= LEG_COCYX_U_L_OFT + LEG_COCYX_AMPLITUDE_DIV_2 ;
	legs.leg[ E_LEG_F_L ].coxaAngle->min					= LEG_COCYX_U_L_OFT ;
	legs.leg[ E_LEG_F_L ].coxaAngle->max					= LEG_COCYX_U_L_OFT + LEG_COCYX_AMPLITUDE ;
	
	legs.leg[ E_LEG_F_L ].femurAngle->currentPosition		= LEG_FEMUR_U_L_OFT ;
	legs.leg[ E_LEG_F_L ].femurAngle->offset				= LEG_FEMUR_U_L_OFT ;
	legs.leg[ E_LEG_F_L ].femurAngle->mid					= LEG_FEMUR_U_L_OFT - LEG_FEMUR_AMPLITUDE_DIV_2 ;
	legs.leg[ E_LEG_F_L ].femurAngle->min					= LEG_FEMUR_U_L_OFT - LEG_FEMUR_AMPLITUDE ;
	legs.leg[ E_LEG_F_L ].femurAngle->max					= LEG_FEMUR_U_L_OFT ;
	
	legs.leg[ E_LEG_F_L ].tibiaAngle->currentPosition		= LEG_TIBIA_U_L_OFT ;
	legs.leg[ E_LEG_F_L ].tibiaAngle->offset				= LEG_TIBIA_U_L_OFT ;
	legs.leg[ E_LEG_F_L ].tibiaAngle->mid					= LEG_TIBIA_U_L_OFT - LEG_TIBIA_AMPLITUDE_DIV_2 ;
	legs.leg[ E_LEG_F_L ].tibiaAngle->min					= LEG_TIBIA_U_L_OFT - LEG_TIBIA_AMPLITUDE ;
	legs.leg[ E_LEG_F_L ].tibiaAngle->max					= LEG_TIBIA_U_L_OFT ;
	
	
	legs.leg[ E_LEG_M_L ].coxaAngle->currentPosition		= LEG_COCYX_M_L_OFT ;
	legs.leg[ E_LEG_M_L ].coxaAngle->offset					= LEG_COCYX_M_L_OFT ;
	legs.leg[ E_LEG_M_L ].coxaAngle->mid					= LEG_COCYX_M_L_OFT ;
	legs.leg[ E_LEG_M_L ].coxaAngle->min					= LEG_COCYX_M_L_OFT - LEG_COCYX_AMPLITUDE_DIV_2 ;
	legs.leg[ E_LEG_M_L ].coxaAngle->max					= LEG_COCYX_M_L_OFT + LEG_COCYX_AMPLITUDE_DIV_2 ;
	
	legs.leg[ E_LEG_M_L ].femurAngle->currentPosition		= LEG_FEMUR_M_L_OFT ;
	legs.leg[ E_LEG_M_L ].femurAngle->offset				= LEG_FEMUR_M_L_OFT ;
	legs.leg[ E_LEG_M_L ].femurAngle->mid					= LEG_FEMUR_M_L_OFT - LEG_FEMUR_AMPLITUDE_DIV_2 ;
	legs.leg[ E_LEG_M_L ].femurAngle->min					= LEG_FEMUR_M_L_OFT - LEG_FEMUR_AMPLITUDE ;
	legs.leg[ E_LEG_M_L ].femurAngle->max					= LEG_FEMUR_M_L_OFT ;
	
	legs.leg[ E_LEG_M_L ].tibiaAngle->currentPosition		= LEG_TIBIA_M_L_OFT ;
	legs.leg[ E_LEG_M_L ].tibiaAngle->offset				= LEG_TIBIA_M_L_OFT ;
	legs.leg[ E_LEG_M_L ].tibiaAngle->mid					= LEG_TIBIA_M_L_OFT - LEG_TIBIA_AMPLITUDE ;
	legs.leg[ E_LEG_M_L ].tibiaAngle->min					= LEG_TIBIA_M_L_OFT - LEG_TIBIA_AMPLITUDE_DIV_2 ;
	legs.leg[ E_LEG_M_L ].tibiaAngle->max					= LEG_TIBIA_M_L_OFT ;
	
	
	legs.leg[ E_LEG_R_L ].coxaAngle->currentPosition		= LEG_COCYX_B_L_OFT ;
	legs.leg[ E_LEG_R_L ].coxaAngle->offset					= LEG_COCYX_B_L_OFT ;
	legs.leg[ E_LEG_R_L ].coxaAngle->mid					= LEG_COCYX_B_L_OFT - LEG_COCYX_AMPLITUDE_DIV_2 ;
	legs.leg[ E_LEG_R_L ].coxaAngle->min					= LEG_COCYX_B_L_OFT - LEG_COCYX_AMPLITUDE ;
	legs.leg[ E_LEG_R_L ].coxaAngle->max					= LEG_COCYX_B_L_OFT ;

	legs.leg[ E_LEG_R_L ].femurAngle->currentPosition		= LEG_FEMUR_B_L_OFT ;
	legs.leg[ E_LEG_R_L ].femurAngle->offset				= LEG_FEMUR_B_L_OFT ;
	legs.leg[ E_LEG_R_L ].femurAngle->mid					= LEG_FEMUR_B_L_OFT - LEG_FEMUR_AMPLITUDE_DIV_2 ;
	legs.leg[ E_LEG_R_L ].femurAngle->min					= LEG_FEMUR_B_L_OFT - LEG_FEMUR_AMPLITUDE ;
	legs.leg[ E_LEG_R_L ].femurAngle->max					= LEG_FEMUR_B_L_OFT ;
	
	legs.leg[ E_LEG_R_L ].tibiaAngle->currentPosition		= LEG_TIBIA_B_L_OFT ;
	legs.leg[ E_LEG_R_L ].tibiaAngle->offset				= LEG_TIBIA_B_L_OFT ;
	legs.leg[ E_LEG_R_L ].tibiaAngle->mid					= LEG_TIBIA_B_L_OFT - LEG_TIBIA_AMPLITUDE_DIV_2 ;
	legs.leg[ E_LEG_R_L ].tibiaAngle->min					= LEG_TIBIA_B_L_OFT - LEG_TIBIA_AMPLITUDE ;
	legs.leg[ E_LEG_R_L ].tibiaAngle->max					= LEG_TIBIA_B_L_OFT ;
	
	
	legs.leg[ E_LEG_F_R ].coxaAngle->currentPosition		= LEG_COCYX_U_R_OFT ;
	legs.leg[ E_LEG_F_R ].coxaAngle->offset					= LEG_COCYX_U_R_OFT ;
	legs.leg[ E_LEG_F_R ].coxaAngle->mid					= LEG_COCYX_U_R_OFT - LEG_COCYX_AMPLITUDE_DIV_2 ;
	legs.leg[ E_LEG_F_R ].coxaAngle->min					= LEG_COCYX_U_R_OFT - LEG_COCYX_AMPLITUDE ;
	legs.leg[ E_LEG_F_R ].coxaAngle->max					= LEG_COCYX_U_R_OFT ;
	
	legs.leg[ E_LEG_F_R ].femurAngle->currentPosition		= LEG_FEMUR_U_R_OFT ;
	legs.leg[ E_LEG_F_R ].femurAngle->offset				= LEG_FEMUR_U_R_OFT ;
	legs.leg[ E_LEG_F_R ].femurAngle->mid					= LEG_FEMUR_U_R_OFT + LEG_FEMUR_AMPLITUDE_DIV_2 ;
	legs.leg[ E_LEG_F_R ].femurAngle->min					= LEG_FEMUR_U_R_OFT ;
	legs.leg[ E_LEG_F_R ].femurAngle->max					= LEG_FEMUR_U_R_OFT + LEG_FEMUR_AMPLITUDE ;
	
	legs.leg[ E_LEG_F_R ].tibiaAngle->currentPosition		= LEG_TIBIA_U_R_OFT ;
	legs.leg[ E_LEG_F_R ].tibiaAngle->offset				= LEG_TIBIA_U_R_OFT ;
	legs.leg[ E_LEG_F_R ].tibiaAngle->mid					= LEG_TIBIA_U_R_OFT + LEG_TIBIA_AMPLITUDE_DIV_2 ;
	legs.leg[ E_LEG_F_R ].tibiaAngle->min					= LEG_TIBIA_U_R_OFT ;
	legs.leg[ E_LEG_F_R ].tibiaAngle->max					= LEG_TIBIA_U_R_OFT + LEG_TIBIA_AMPLITUDE ;
	
			  
	legs.leg[ E_LEG_M_R ].coxaAngle->currentPosition		= LEG_COCYX_M_R_OFT ;
	legs.leg[ E_LEG_M_R ].coxaAngle->offset					= LEG_COCYX_M_R_OFT ;
	legs.leg[ E_LEG_M_R ].coxaAngle->mid					= LEG_COCYX_M_R_OFT ;
	legs.leg[ E_LEG_M_R ].coxaAngle->min					= LEG_COCYX_M_R_OFT - LEG_COCYX_AMPLITUDE_DIV_2 ;
	legs.leg[ E_LEG_M_R ].coxaAngle->max					= LEG_COCYX_M_R_OFT + LEG_COCYX_AMPLITUDE_DIV_2 ;
	
	legs.leg[ E_LEG_M_R ].femurAngle->currentPosition		= LEG_FEMUR_M_R_OFT ;
	legs.leg[ E_LEG_M_R ].femurAngle->offset				= LEG_FEMUR_M_R_OFT ;
	legs.leg[ E_LEG_M_R ].femurAngle->mid					= LEG_FEMUR_M_R_OFT + LEG_FEMUR_AMPLITUDE_DIV_2 ;
	legs.leg[ E_LEG_M_R ].femurAngle->min					= LEG_FEMUR_M_R_OFT ;
	legs.leg[ E_LEG_M_R ].femurAngle->max					= LEG_FEMUR_M_R_OFT + LEG_FEMUR_AMPLITUDE ;
	
	legs.leg[ E_LEG_M_R ].tibiaAngle->currentPosition		= LEG_TIBIA_M_R_OFT ;
	legs.leg[ E_LEG_M_R ].tibiaAngle->offset				= LEG_TIBIA_M_R_OFT ;
	legs.leg[ E_LEG_M_R ].tibiaAngle->mid					= LEG_TIBIA_M_R_OFT + LEG_TIBIA_AMPLITUDE_DIV_2 ;
	legs.leg[ E_LEG_M_R ].tibiaAngle->min					= LEG_TIBIA_M_R_OFT ;
	legs.leg[ E_LEG_M_R ].tibiaAngle->max					= LEG_TIBIA_M_R_OFT + LEG_TIBIA_AMPLITUDE ;
	
	
	legs.leg[ E_LEG_R_R ].coxaAngle->currentPosition		= LEG_COCYX_B_R_OFT ;
	legs.leg[ E_LEG_R_R ].coxaAngle->offset					= LEG_COCYX_B_R_OFT ;
	legs.leg[ E_LEG_R_R ].coxaAngle->mid					= LEG_COCYX_B_R_OFT + LEG_COCYX_AMPLITUDE_DIV_2 ;
	legs.leg[ E_LEG_R_R ].coxaAngle->min					= LEG_COCYX_B_R_OFT ;
	legs.leg[ E_LEG_R_R ].coxaAngle->max					= LEG_COCYX_B_R_OFT + LEG_COCYX_AMPLITUDE ;

	legs.leg[ E_LEG_R_R ].femurAngle->currentPosition		= LEG_FEMUR_B_R_OFT ;
	legs.leg[ E_LEG_R_R ].femurAngle->offset				= LEG_FEMUR_B_R_OFT ;
	legs.leg[ E_LEG_R_R ].femurAngle->mid					= LEG_FEMUR_B_R_OFT + LEG_FEMUR_AMPLITUDE_DIV_2 ;
	legs.leg[ E_LEG_R_R ].femurAngle->min					= LEG_FEMUR_B_R_OFT ;
	legs.leg[ E_LEG_R_R ].femurAngle->max					= LEG_FEMUR_B_R_OFT + LEG_FEMUR_AMPLITUDE ;
	
	legs.leg[ E_LEG_R_R ].tibiaAngle->currentPosition		= LEG_TIBIA_B_R_OFT ;
	legs.leg[ E_LEG_R_R ].tibiaAngle->offset				= LEG_TIBIA_B_R_OFT ;
	legs.leg[ E_LEG_R_R ].tibiaAngle->mid					= LEG_TIBIA_B_R_OFT + LEG_TIBIA_AMPLITUDE_DIV_2 ;
	legs.leg[ E_LEG_R_R ].tibiaAngle->min					= LEG_TIBIA_B_R_OFT ;
	legs.leg[ E_LEG_R_R ].tibiaAngle->max					= LEG_TIBIA_B_R_OFT + LEG_TIBIA_AMPLITUDE ;
	return TRUE;
}