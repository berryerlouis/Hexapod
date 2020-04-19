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
#define SERVO_INTIALIZATION_PERIOD	250U

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
static SLegs legs;

//period of refresh
uint32_t prevMillisServosInitialize = 0U;

uint8_t servoIndex = 0U;

//variable for trigonometry
static volatile float L, L1;
static volatile float alpha, alpha1,alpha2,beta,gama;

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
//permits to set offset of bot
static Boolean DrvLegSetOffset ( void ) ;
static void DrvLegInititalizing( void );
static Boolean DrvLegInverseKinematics( ELeg indexLeg, float x, float y, float z, Int16U speed );
/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////

// Init of Drv PWM
Boolean DrvLegInit( void )
{
	//permits to enable servo step by step
	servoIndex = 0U;
	legs.isInitialized = FALSE;
	
	//initialize the driver
	DrvServoInit();
	
	//get each servos for each leg and set member id
	for(Int8U i = 0U ; i < NB_LEGS ; i++ )
	{
		legs.leg[i].id				= i ;
		legs.leg[i].enable			= TRUE ;
		legs.leg[i].initialized		= FALSE ;
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

SLeg* DrvLegGetStruct( ELeg indexLeg )
{
	return &legs.leg[indexLeg];
}

Boolean DrvLegSetTarget( ELeg indexLeg, float x, float y, float z, Int16U speed  )
{
	//set the next targets
	legs.leg[ indexLeg ].targetPositionX = x;  
	legs.leg[ indexLeg ].targetPositionY = y; 
	legs.leg[ indexLeg ].targetPositionZ = z; 
	legs.leg[ indexLeg ].startPositionX = legs.leg[ indexLeg ].currentPositionX; 
	legs.leg[ indexLeg ].startPositionY = legs.leg[ indexLeg ].currentPositionY;
	legs.leg[ indexLeg ].startPositionZ = legs.leg[ indexLeg ].currentPositionZ;
	legs.leg[ indexLeg ].startTime = DrvTickGetTimeMs();
	legs.leg[ indexLeg ].speed = speed;
	return TRUE;
}

Boolean DrvLegCheckTarget( ELeg indexLeg )
{
	if((legs.leg[ indexLeg ].currentPositionX == legs.leg[ indexLeg ].targetPositionX)
	 && (legs.leg[ indexLeg ].currentPositionY == legs.leg[ indexLeg ].targetPositionY)
	 && (legs.leg[ indexLeg ].currentPositionZ == legs.leg[ indexLeg ].targetPositionZ))
	{
		return TRUE;
	}
	return FALSE;
}

Boolean DrvLegGetXYZ(ELeg indexLeg, float *x, float *y, float *z )
{
	x[0] = legs.leg[ indexLeg ].currentPositionX;
	y[0] = legs.leg[ indexLeg ].currentPositionY;
	z[0] = legs.leg[ indexLeg ].currentPositionZ;
	return TRUE;
}


Boolean DrvLegSetPosition( ELeg indexLeg, Int16S coxaAngle, Int16S femurAngle, Int16S tibiaAngle, Int16U speed )
{
	if(
		(indexLeg < E_NB_LEGS) &&
		(legs.leg[indexLeg].initialized == TRUE )&&
		(legs.leg[indexLeg].enable == TRUE )
	)
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
	return legs.isInitialized;
}

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
		//compute next current position for each leg
		for(Int8U indexLeg = 0U ; indexLeg < NB_LEGS ; indexLeg++ )
		{
			//if needed to update leg
			if( !DrvLegCheckTarget(indexLeg) )
			{
				Int32U currentTime	= DrvTickGetTimeMs() - legs.leg[ indexLeg ].startTime;
				float duration		= legs.leg[ indexLeg ].speed;
		
				if(legs.leg[ indexLeg ].currentPositionX != legs.leg[ indexLeg ].targetPositionX)
				{
					legs.leg[ indexLeg ].currentPositionX = (-(legs.leg[ indexLeg ].targetPositionX - legs.leg[ indexLeg ].startPositionX)/2.0) * (cos( M_PI * (currentTime / duration)) - 1.0 ) + legs.leg[ indexLeg ].startPositionX;
				}
		
				if(legs.leg[ indexLeg ].currentPositionY != legs.leg[ indexLeg ].targetPositionY)
				{
					legs.leg[ indexLeg ].currentPositionY = (-(legs.leg[ indexLeg ].targetPositionY - legs.leg[ indexLeg ].startPositionY)/2.0) * (cos( M_PI * (currentTime / duration)) - 1.0 ) + legs.leg[ indexLeg ].startPositionY;
				}
		
				if(legs.leg[ indexLeg ].currentPositionZ !=legs.leg[ indexLeg ].targetPositionZ)
				{
					legs.leg[ indexLeg ].currentPositionZ = (-(legs.leg[ indexLeg ].targetPositionZ - legs.leg[ indexLeg ].startPositionZ)/2.0) * (cos( M_PI * (currentTime / duration)) - 1.0 ) + legs.leg[ indexLeg ].startPositionZ;
				}
		
				//if time is over set the current position to target
				if(currentTime >= duration)
				{
					legs.leg[ indexLeg ].currentPositionX = legs.leg[ indexLeg ].targetPositionX;
					legs.leg[ indexLeg ].currentPositionY = legs.leg[ indexLeg ].targetPositionY;
					legs.leg[ indexLeg ].currentPositionZ = legs.leg[ indexLeg ].targetPositionZ;
				}
				
				//apply new position
				DrvLegInverseKinematics(indexLeg,
					legs.leg[ indexLeg ].currentPositionX,
					legs.leg[ indexLeg ].currentPositionY,
					legs.leg[ indexLeg ].currentPositionZ,
					0);
			}
		}
	}
	return TRUE;
}

float DrvLegGetXFromCoxaAngle( float angle , float y)
{
	return (y + LEG_COCYX_LENGTH) * tan(ToRad(angle));
}

static Boolean DrvLegInverseKinematics( ELeg indexLeg, float x, float y, float z, Int16U speed )
{	
	//http://te.unib.ac.id/lecturer/indraagustian/2014/05/3dof-inverse-kinematic-for-armleg-of-robot-use-arduino/
	if(
		(indexLeg < E_NB_LEGS) &&
		(legs.leg[indexLeg].initialized == TRUE )&&
		(legs.leg[indexLeg].enable == TRUE )
	)
	{		
		//index of servos
		Int8U indexHanche = 0U + (indexLeg * 3U);
		Int8U indexGenoux  = 1U + (indexLeg * 3U);
		Int8U indexCheville = 2U + (indexLeg * 3U);
	
		//add size of members
		x += 0;
		y += LEG_COCYX_LENGTH;
		z += (LEG_TIBIA_LENGTH - LEG_FEMUR_LENGTH);
		
		//compute angles from distances
		L1 = (float)sqrt((float)((float)(x * x) + (float)(y * y)));
		gama = (float)atan((float)x / (float)y);
		L = (float)sqrt(((L1 - LEG_COCYX_LENGTH) * (L1 - LEG_COCYX_LENGTH)) + (float)(z * z));
		beta = (float)acos(((LEG_TIBIA_LENGTH * LEG_TIBIA_LENGTH) + (LEG_FEMUR_LENGTH * LEG_FEMUR_LENGTH) - (L * L)) / (2 * LEG_TIBIA_LENGTH * LEG_FEMUR_LENGTH));
		alpha1 = (float)acos((float)z / L);
		alpha2 = (float)acos(((LEG_FEMUR_LENGTH * LEG_FEMUR_LENGTH) + (L * L) - (LEG_TIBIA_LENGTH * LEG_TIBIA_LENGTH)) / (2 * LEG_FEMUR_LENGTH * L));
		alpha = alpha1 + alpha2;
	
		gama  = (gama * 180) / M_PI;
		alpha = (alpha * 180) / M_PI;
		beta  = (beta * 180) / M_PI;
	
		//set servos position
		uint8_t reponse = 0;
		
		//we have to split left and right side because of servo min and max are reverted between the 2 sides
		//left side
		if((indexLeg == E_LEG_U_L)||(indexLeg == E_LEG_M_L)||(indexLeg == E_LEG_B_L))
		{
			//limits and invert beta
			alpha = alpha - 180 + legs.leg[ indexLeg ].femurAngle->max;
			beta = -1 * beta + legs.leg[ indexLeg ].tibiaAngle->max;
		}
		//right side
		else
		{
			//limits and invert beta
			alpha = -alpha + 180 + legs.leg[ indexLeg ].femurAngle->min;
			beta = beta + legs.leg[ indexLeg ].tibiaAngle->min;
		}
		
		//same for the gama angle
		//same for the gama angle
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
		
		//pre check before sending to servos, make sure that all servos could move
		if(
			( gama >= legs.leg[ indexLeg ].coxaAngle->min ) && ( gama <= legs.leg[ indexLeg ].coxaAngle->max ) &&
			( alpha >= legs.leg[ indexLeg ].femurAngle->min ) && ( alpha <= legs.leg[ indexLeg ].femurAngle->max ) &&
			( beta >= legs.leg[ indexLeg ].tibiaAngle->min ) && ( beta <= legs.leg[ indexLeg ].tibiaAngle->max )
			)
		{
			//send to servo
			reponse = DrvServoSetTarget(indexHanche,	(Int16S)round(gama)	,	speed );
			reponse *= 10U;
			reponse += DrvServoSetTarget(indexGenoux,	(Int16S)(round(alpha)),	speed );
			reponse *= 10U;
			reponse += DrvServoSetTarget(indexCheville,	(Int16S)(round(beta) ),	speed );
			
			if(reponse == 111U)
			{
				legs.leg[ indexLeg ].currentPositionX = x;
				legs.leg[ indexLeg ].currentPositionY = y - LEG_COCYX_LENGTH;
				legs.leg[ indexLeg ].currentPositionZ = z - (LEG_TIBIA_LENGTH - LEG_FEMUR_LENGTH);
				return TRUE;
			}
			
			
			return FALSE;
		}
	}
	return FALSE;
}


static void DrvLegInititalizing( void )
{
	//if initializing time
	if((DrvTickGetTimeMs() - prevMillisServosInitialize) > SERVO_INTIALIZATION_PERIOD)
	{
		if(servoIndex < NB_SERVOS )
		{
			if(servoIndex % 3 == 0)
			{
				//get x from angles
				legs.leg[servoIndex / NB_SERVOS_PER_LEG].currentPositionX = DrvServoGetTarget(servoIndex) ;
				legs.leg[servoIndex / NB_SERVOS_PER_LEG].targetPositionX = DrvServoGetTarget(servoIndex) ;
				legs.leg[servoIndex / NB_SERVOS_PER_LEG].startPositionX = DrvServoGetTarget(servoIndex) ;
			}
			
			//leg is now initialized
			legs.leg[servoIndex / NB_SERVOS_PER_LEG].initialized = TRUE ;
			//enable servo
			DrvServoActivate(servoIndex,TRUE);
		}
		else
		{
			//initialization done
			legs.isInitialized = TRUE;
		}
		//prepare for next servo
		servoIndex++;
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
	legs.leg[ E_LEG_U_L ].coxaAngle->currentPosition		= LEG_COCYX_U_L_MID ;
	legs.leg[ E_LEG_U_L ].coxaAngle->targetPosition		= LEG_COCYX_U_L_MID ;
	legs.leg[ E_LEG_U_L ].coxaAngle->offset			= LEG_COCYX_U_L_OFT ;
	legs.leg[ E_LEG_U_L ].coxaAngle->mid			= LEG_COCYX_U_L_MID ;
	legs.leg[ E_LEG_U_L ].coxaAngle->min			= LEG_COCYX_U_L_MIN ;
	legs.leg[ E_LEG_U_L ].coxaAngle->max			= LEG_COCYX_U_L_MAX ;
	legs.leg[ E_LEG_U_L ].coxaAngle->ease			= E_SERVO_EASE_SINUS_OUT ;
	legs.leg[ E_LEG_U_L ].femurAngle->currentPosition		= LEG_FEMUR_U_L_MAX ;
	legs.leg[ E_LEG_U_L ].femurAngle->targetPosition		= LEG_FEMUR_U_L_MAX ;
	legs.leg[ E_LEG_U_L ].femurAngle->offset			= LEG_FEMUR_U_L_OFT ;
	legs.leg[ E_LEG_U_L ].femurAngle->mid			= LEG_FEMUR_U_L_MID ;
	legs.leg[ E_LEG_U_L ].femurAngle->min			= LEG_FEMUR_U_L_MIN ;
	legs.leg[ E_LEG_U_L ].femurAngle->max			= LEG_FEMUR_U_L_MAX ;
	legs.leg[ E_LEG_U_L ].femurAngle->ease			= E_SERVO_EASE_SINUS_OUT ;
	legs.leg[ E_LEG_U_L ].tibiaAngle->currentPosition		= LEG_TIBIA_U_L_MAX ;
	legs.leg[ E_LEG_U_L ].tibiaAngle->targetPosition		= LEG_TIBIA_U_L_MAX ;
	legs.leg[ E_LEG_U_L ].tibiaAngle->offset			= LEG_TIBIA_U_L_OFT ;
	legs.leg[ E_LEG_U_L ].tibiaAngle->mid			= LEG_TIBIA_U_L_MID ;
	legs.leg[ E_LEG_U_L ].tibiaAngle->min			= LEG_TIBIA_U_L_MIN ;
	legs.leg[ E_LEG_U_L ].tibiaAngle->max			= LEG_TIBIA_U_L_MAX ;
	legs.leg[ E_LEG_U_L ].tibiaAngle->ease			= E_SERVO_EASE_SINUS_IN_OUT ;
	
	legs.leg[ E_LEG_M_L ].coxaAngle->currentPosition		= LEG_COCYX_M_L_MID ;
	legs.leg[ E_LEG_M_L ].coxaAngle->targetPosition		= LEG_COCYX_M_L_MID ;
	legs.leg[ E_LEG_M_L ].coxaAngle->offset			= LEG_COCYX_M_L_OFT ;
	legs.leg[ E_LEG_M_L ].coxaAngle->mid			= LEG_COCYX_M_L_MID ;
	legs.leg[ E_LEG_M_L ].coxaAngle->min			= LEG_COCYX_M_L_MIN ;
	legs.leg[ E_LEG_M_L ].coxaAngle->max			= LEG_COCYX_M_L_MAX ;
	legs.leg[ E_LEG_M_L ].coxaAngle->ease			= E_SERVO_EASE_SINUS_OUT ;
	legs.leg[ E_LEG_M_L ].femurAngle->currentPosition		= LEG_FEMUR_M_L_MAX ;
	legs.leg[ E_LEG_M_L ].femurAngle->targetPosition		= LEG_FEMUR_M_L_MAX ;
	legs.leg[ E_LEG_M_L ].femurAngle->offset			= LEG_FEMUR_M_L_OFT ;
	legs.leg[ E_LEG_M_L ].femurAngle->mid			= LEG_FEMUR_M_L_MID ;
	legs.leg[ E_LEG_M_L ].femurAngle->min			= LEG_FEMUR_M_L_MIN ;
	legs.leg[ E_LEG_M_L ].femurAngle->max			= LEG_FEMUR_M_L_MAX ;
	legs.leg[ E_LEG_M_L ].femurAngle->ease			= E_SERVO_EASE_SINUS_OUT ;
	legs.leg[ E_LEG_M_L ].tibiaAngle->currentPosition		= LEG_TIBIA_M_L_MAX ;
	legs.leg[ E_LEG_M_L ].tibiaAngle->targetPosition		= LEG_TIBIA_M_L_MAX ;
	legs.leg[ E_LEG_M_L ].tibiaAngle->offset			= LEG_TIBIA_M_L_OFT ;
	legs.leg[ E_LEG_M_L ].tibiaAngle->mid			= LEG_TIBIA_M_L_MID ;
	legs.leg[ E_LEG_M_L ].tibiaAngle->min			= LEG_TIBIA_M_L_MIN ;
	legs.leg[ E_LEG_M_L ].tibiaAngle->max			= LEG_TIBIA_M_L_MAX ;
	legs.leg[ E_LEG_M_L ].tibiaAngle->ease			= E_SERVO_EASE_SINUS_IN_OUT ;
	
	legs.leg[ E_LEG_B_L ].coxaAngle->currentPosition		= LEG_COCYX_B_L_MID ;
	legs.leg[ E_LEG_B_L ].coxaAngle->targetPosition		= LEG_COCYX_B_L_MID ;
	legs.leg[ E_LEG_B_L ].coxaAngle->offset			= LEG_COCYX_B_L_OFT ;
	legs.leg[ E_LEG_B_L ].coxaAngle->mid			= LEG_COCYX_B_L_MID ;
	legs.leg[ E_LEG_B_L ].coxaAngle->min			= LEG_COCYX_B_L_MIN ;
	legs.leg[ E_LEG_B_L ].coxaAngle->max			= LEG_COCYX_B_L_MAX ;
	legs.leg[ E_LEG_B_L ].coxaAngle->ease			= E_SERVO_EASE_SINUS_OUT ;
	legs.leg[ E_LEG_B_L ].femurAngle->currentPosition		= LEG_FEMUR_B_L_MAX ;
	legs.leg[ E_LEG_B_L ].femurAngle->targetPosition		= LEG_FEMUR_B_L_MAX ;
	legs.leg[ E_LEG_B_L ].femurAngle->offset			= LEG_FEMUR_B_L_OFT ;
	legs.leg[ E_LEG_B_L ].femurAngle->mid			= LEG_FEMUR_B_L_MID ;
	legs.leg[ E_LEG_B_L ].femurAngle->min			= LEG_FEMUR_B_L_MIN ;
	legs.leg[ E_LEG_B_L ].femurAngle->max			= LEG_FEMUR_B_L_MAX ;
	legs.leg[ E_LEG_B_L ].femurAngle->ease			= E_SERVO_EASE_SINUS_OUT ;
	legs.leg[ E_LEG_B_L ].tibiaAngle->currentPosition		= LEG_TIBIA_B_L_MAX ;
	legs.leg[ E_LEG_B_L ].tibiaAngle->targetPosition		= LEG_TIBIA_B_L_MAX ;
	legs.leg[ E_LEG_B_L ].tibiaAngle->offset			= LEG_TIBIA_B_L_OFT ;
	legs.leg[ E_LEG_B_L ].tibiaAngle->mid			= LEG_TIBIA_B_L_MID ;
	legs.leg[ E_LEG_B_L ].tibiaAngle->min			= LEG_TIBIA_B_L_MIN ;
	legs.leg[ E_LEG_B_L ].tibiaAngle->max			= LEG_TIBIA_B_L_MAX ;
	legs.leg[ E_LEG_B_L ].tibiaAngle->ease			= E_SERVO_EASE_SINUS_IN_OUT ;
	
	legs.leg[ E_LEG_U_R ].coxaAngle->currentPosition		= LEG_COCYX_U_R_MID ;
	legs.leg[ E_LEG_U_R ].coxaAngle->targetPosition		= LEG_COCYX_U_R_MID ;
	legs.leg[ E_LEG_U_R ].coxaAngle->offset			= LEG_COCYX_U_R_OFT ;
	legs.leg[ E_LEG_U_R ].coxaAngle->mid			= LEG_COCYX_U_R_MID ;
	legs.leg[ E_LEG_U_R ].coxaAngle->min			= LEG_COCYX_U_R_MIN ;
	legs.leg[ E_LEG_U_R ].coxaAngle->max			= LEG_COCYX_U_R_MAX ;
	legs.leg[ E_LEG_U_R ].coxaAngle->ease			= E_SERVO_EASE_SINUS_OUT ;
	legs.leg[ E_LEG_U_R ].femurAngle->currentPosition		= LEG_FEMUR_U_R_MIN ;
	legs.leg[ E_LEG_U_R ].femurAngle->targetPosition		= LEG_FEMUR_U_R_MIN ;
	legs.leg[ E_LEG_U_R ].femurAngle->offset			= LEG_FEMUR_U_R_OFT ;
	legs.leg[ E_LEG_U_R ].femurAngle->mid			= LEG_FEMUR_U_R_MID ;
	legs.leg[ E_LEG_U_R ].femurAngle->min			= LEG_FEMUR_U_R_MIN ;
	legs.leg[ E_LEG_U_R ].femurAngle->max			= LEG_FEMUR_U_R_MAX ;
	legs.leg[ E_LEG_U_R ].femurAngle->ease			= E_SERVO_EASE_SINUS_OUT ;
	legs.leg[ E_LEG_U_R ].tibiaAngle->currentPosition		= LEG_TIBIA_U_R_MIN ;
	legs.leg[ E_LEG_U_R ].tibiaAngle->targetPosition		= LEG_TIBIA_U_R_MIN ;
	legs.leg[ E_LEG_U_R ].tibiaAngle->offset			= LEG_TIBIA_U_R_OFT ;
	legs.leg[ E_LEG_U_R ].tibiaAngle->mid			= LEG_TIBIA_U_R_MID ;
	legs.leg[ E_LEG_U_R ].tibiaAngle->min			= LEG_TIBIA_U_R_MIN ;
	legs.leg[ E_LEG_U_R ].tibiaAngle->max			= LEG_TIBIA_U_R_MAX ;
	legs.leg[ E_LEG_U_R ].tibiaAngle->ease			= E_SERVO_EASE_SINUS_IN_OUT ;
			  
	legs.leg[ E_LEG_M_R ].coxaAngle->currentPosition		= LEG_COCYX_M_R_MID ;
	legs.leg[ E_LEG_M_R ].coxaAngle->targetPosition		= LEG_COCYX_M_R_MID ;
	legs.leg[ E_LEG_M_R ].coxaAngle->offset			= LEG_COCYX_M_R_OFT ;
	legs.leg[ E_LEG_M_R ].coxaAngle->mid			= LEG_COCYX_M_R_MID ;
	legs.leg[ E_LEG_M_R ].coxaAngle->min			= LEG_COCYX_M_R_MIN ;
	legs.leg[ E_LEG_M_R ].coxaAngle->max			= LEG_COCYX_M_R_MAX ;
	legs.leg[ E_LEG_M_R ].coxaAngle->ease			= E_SERVO_EASE_SINUS_OUT ;
	legs.leg[ E_LEG_M_R ].femurAngle->currentPosition		= LEG_FEMUR_M_R_MIN ;
	legs.leg[ E_LEG_M_R ].femurAngle->targetPosition		= LEG_FEMUR_M_R_MIN ;
	legs.leg[ E_LEG_M_R ].femurAngle->offset			= LEG_FEMUR_M_R_OFT ;
	legs.leg[ E_LEG_M_R ].femurAngle->mid			= LEG_FEMUR_M_R_MID ;
	legs.leg[ E_LEG_M_R ].femurAngle->min			= LEG_FEMUR_M_R_MIN ;
	legs.leg[ E_LEG_M_R ].femurAngle->max			= LEG_FEMUR_M_R_MAX ;
	legs.leg[ E_LEG_M_R ].femurAngle->ease			= E_SERVO_EASE_SINUS_OUT ;
	legs.leg[ E_LEG_M_R ].tibiaAngle->currentPosition		= LEG_TIBIA_M_R_MIN ;
	legs.leg[ E_LEG_M_R ].tibiaAngle->targetPosition		= LEG_TIBIA_M_R_MIN ;
	legs.leg[ E_LEG_M_R ].tibiaAngle->offset			= LEG_TIBIA_M_R_OFT ;
	legs.leg[ E_LEG_M_R ].tibiaAngle->mid			= LEG_TIBIA_M_R_MID ;
	legs.leg[ E_LEG_M_R ].tibiaAngle->min			= LEG_TIBIA_M_R_MIN ;
	legs.leg[ E_LEG_M_R ].tibiaAngle->max			= LEG_TIBIA_M_R_MAX ;
	legs.leg[ E_LEG_M_R ].tibiaAngle->ease			= E_SERVO_EASE_SINUS_IN_OUT ;
			  
	legs.leg[ E_LEG_B_R ].coxaAngle->currentPosition		= LEG_COCYX_B_R_MID ;
	legs.leg[ E_LEG_B_R ].coxaAngle->targetPosition		= LEG_COCYX_B_R_MID ;
	legs.leg[ E_LEG_B_R ].coxaAngle->offset			= LEG_COCYX_B_R_OFT ;
	legs.leg[ E_LEG_B_R ].coxaAngle->mid			= LEG_COCYX_B_R_MID ;
	legs.leg[ E_LEG_B_R ].coxaAngle->min			= LEG_COCYX_B_R_MIN ;
	legs.leg[ E_LEG_B_R ].coxaAngle->max			= LEG_COCYX_B_R_MAX ;
	legs.leg[ E_LEG_B_R ].coxaAngle->ease			= E_SERVO_EASE_SINUS_OUT ;
	legs.leg[ E_LEG_B_R ].femurAngle->currentPosition		= LEG_FEMUR_B_R_MIN ;
	legs.leg[ E_LEG_B_R ].femurAngle->targetPosition		= LEG_FEMUR_B_R_MIN ;
	legs.leg[ E_LEG_B_R ].femurAngle->offset			= LEG_FEMUR_B_R_OFT ;
	legs.leg[ E_LEG_B_R ].femurAngle->mid			= LEG_FEMUR_B_R_MID ;
	legs.leg[ E_LEG_B_R ].femurAngle->min			= LEG_FEMUR_B_R_MIN ;
	legs.leg[ E_LEG_B_R ].femurAngle->max			= LEG_FEMUR_B_R_MAX ;
	legs.leg[ E_LEG_B_R ].femurAngle->ease			= E_SERVO_EASE_SINUS_OUT ;
	legs.leg[ E_LEG_B_R ].tibiaAngle->currentPosition		= LEG_TIBIA_B_R_MIN ;
	legs.leg[ E_LEG_B_R ].tibiaAngle->targetPosition		= LEG_TIBIA_B_R_MIN ;
	legs.leg[ E_LEG_B_R ].tibiaAngle->offset			= LEG_TIBIA_B_R_OFT ;
	legs.leg[ E_LEG_B_R ].tibiaAngle->mid			= LEG_TIBIA_B_R_MID ;
	legs.leg[ E_LEG_B_R ].tibiaAngle->min			= LEG_TIBIA_B_R_MIN ;
	legs.leg[ E_LEG_B_R ].tibiaAngle->max			= LEG_TIBIA_B_R_MAX ;
	legs.leg[ E_LEG_B_R ].tibiaAngle->ease			= E_SERVO_EASE_SINUS_IN_OUT ;
		
	return TRUE;
}