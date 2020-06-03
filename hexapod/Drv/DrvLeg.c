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

uint8_t servoIndexInitialization = 0U;


////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
//permits to set offset of bot
static Boolean DrvLegSetOffset ( void ) ;
static void DrvLegInititalizing( void );
static Boolean DrvLegApplyPosition( ELeg indexLeg, float coxaAngle, float femurAngle, float tibiaAngle, Int16U speed );
static float DrvLegEase( E_SERVO_EASES ease, float target, float start, Int32U startTime, Int32U duration);
/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////

// Init of Drv PWM
Boolean DrvLegInit( void )
{
	//permits to enable servo step by step
	servoIndexInitialization = 0U;
	legs.isInitialized = FALSE;
	
	//initialize the driver
	DrvServoInit();
	
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

Boolean DrvLegSetTarget( ELeg indexLeg, float x, float y, float z, Int16U speed  )
{
	//set the next targets
	legs.leg[ indexLeg ].targetPositionX = x;  
	legs.leg[ indexLeg ].targetPositionY = y;
	legs.leg[ indexLeg ].targetPositionZ = z; 
	legs.leg[ indexLeg ].startTime = DrvTickGetTimeMs();
	legs.leg[ indexLeg ].speed = speed;
	legs.leg[ indexLeg ].newPosition = TRUE;
	return TRUE;
}

Boolean DrvLegSetSpeed( ELeg indexLeg, Int16U speed  )
{
	legs.leg[ indexLeg ].speed = speed;
	return TRUE;
}



Boolean DrvLegCheckTarget( ELeg indexLeg )
{
	if((legs.leg[ indexLeg ].coxaAngle->currentPosition == legs.leg[ indexLeg ].coxaAngle->targetPosition) &&
	   (legs.leg[ indexLeg ].femurAngle->currentPosition == legs.leg[ indexLeg ].femurAngle->targetPosition) &&
	   (legs.leg[ indexLeg ].tibiaAngle->currentPosition == legs.leg[ indexLeg ].tibiaAngle->targetPosition))
	{
		return TRUE;
	}
	return FALSE;
}

Boolean DrvLegGetXYZ(ELeg indexLeg, float *x, float *y, float *z )
{
	x[0] = legs.leg[ indexLeg ].targetPositionX;
	y[0] = legs.leg[ indexLeg ].targetPositionY;
	z[0] = legs.leg[ indexLeg ].targetPositionZ;
	return TRUE;
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
		for(Int8U indexLeg = 0U ; indexLeg < E_NB_LEGS ; indexLeg++ )
		{
				
			if((DrvLegCheckTarget(indexLeg) == FALSE) || (legs.leg[ indexLeg ].newPosition))
			{
				legs.leg[ indexLeg ].newPosition = FALSE;
				float coxaAngle  = 0;
				float femurAngle = 0;
				float tibiaAngle = 0;
				DrvLegComputeInverseKinematics(
					legs.leg[ indexLeg ].targetPositionX,
					legs.leg[ indexLeg ].targetPositionY,
					legs.leg[ indexLeg ].targetPositionZ,
					&coxaAngle,
					&femurAngle,
					&tibiaAngle);
				//apply new position
				DrvLegApplyPosition(indexLeg,
					coxaAngle,
					femurAngle ,
					tibiaAngle,
					legs.leg[ indexLeg ].speed);
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


Boolean DrvLegComputeInverseKinematics(float x, float y, float z, float *coxaAngle, float *femurAngle, float *tibiaAngle )
{
	//variable for trigonometry
	float L, L1;
	float alpha, alpha1,alpha2,beta,gama;
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
	
	coxaAngle[0U] = gama * 10;
	femurAngle[0U] = alpha * 10;
	tibiaAngle[0U] = beta * 10;
	return TRUE;
}

static float DrvLegEase( E_SERVO_EASES ease, float target, float start, Int32U startTime, Int32U duration)
{
	float currentTime	= DrvTickGetTimeMs() - startTime;
	float changeValue	= target - start;
	float current = 0;
	if(ease == E_SERVO_EASE_LINEAR)
	{
		current = changeValue * (currentTime / duration) + start;
	}
	else if(ease == E_SERVO_EASE_SINUS_IN)
	{
		current = -changeValue * (cos( M_PI_2 * (currentTime / duration))) + changeValue + start;
	}
	else if(ease == E_SERVO_EASE_SINUS_OUT)
	{
		current = changeValue * (sin( M_PI_2 * (currentTime / duration))) + start;
	}
	else if(ease == E_SERVO_EASE_SINUS_IN_OUT)
	{
		current = (-changeValue/2.0) * (cos( M_PI * (currentTime / duration)) - 1.0 ) + start;
	}
	
	//if time is over set the current position to target
	if(currentTime >= duration)
	{
		return target;
	}
	
	return current;
}

static Boolean DrvLegApplyPosition( ELeg indexLeg, float coxaAngle, float femurAngle, float tibiaAngle, Int16U speed )
{	
	//http://te.unib.ac.id/lecturer/indraagustian/2014/05/3dof-inverse-kinematic-for-armleg-of-robot-use-arduino/
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
		float coxaAngleTemp = coxaAngle;
		float femurAngleTemp = femurAngle;
		float tibiaAngleTemp = tibiaAngle;
		//set servos position
		uint8_t reponse = 0;
				
		//we have to split left and right side because of servo min and max are reverted between the 2 sides
		//left side
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
		else if((indexLeg == E_LEG_R_L)||(indexLeg == E_LEG_F_R))
		{
			coxaAngleTemp += legs.leg[ indexLeg ].coxaAngle->max;
		}
		else if((indexLeg == E_LEG_M_L)||(indexLeg == E_LEG_M_R))
		{
			coxaAngleTemp += legs.leg[ indexLeg ].coxaAngle->mid;
		}
		//pre check before sending to servos, make sure that all servos could move
		if(
			( coxaAngleTemp >= legs.leg[ indexLeg ].coxaAngle->min ) && ( coxaAngleTemp <= legs.leg[ indexLeg ].coxaAngle->max ) &&
			( femurAngleTemp >= legs.leg[ indexLeg ].femurAngle->min ) && ( femurAngleTemp <= legs.leg[ indexLeg ].femurAngle->max ) &&
			( tibiaAngleTemp >= legs.leg[ indexLeg ].tibiaAngle->min ) && ( tibiaAngleTemp <= legs.leg[ indexLeg ].tibiaAngle->max )
			)
		{
			Int16S coxaAngleTemp2 = (Int16S)round(coxaAngleTemp);
			Int16S femurAngleTemp2 = (Int16S)round(femurAngleTemp);
			Int16S tibiaAngleTemp2 = (Int16S)round(tibiaAngleTemp);
			//send to servo
			reponse = DrvServoSetTarget(indexHanche,	coxaAngleTemp2,	speed );
			reponse *= 10U;
			reponse += DrvServoSetTarget(indexGenoux,	femurAngleTemp2,	speed );
			reponse *= 10U;
			reponse += DrvServoSetTarget(indexCheville,	tibiaAngleTemp2,	speed );
			
			if(reponse == 111U)
			{
				return TRUE;
			}
			return FALSE;
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
		if(servoIndexInitialization < NB_SERVOS_PER_LEG * E_NB_LEGS )
		{
			if(servoIndexInitialization % 3 == 0)
			{
				//get x from angles
				legs.leg[servoIndexInitialization / NB_SERVOS_PER_LEG].targetPositionX = DrvServoGetTarget(servoIndexInitialization) / 10  ;
			}
			else if(servoIndexInitialization % 3 == 1)
			{
				//get x from angles
				legs.leg[servoIndexInitialization / NB_SERVOS_PER_LEG].targetPositionY = DrvServoGetTarget(servoIndexInitialization) / 10  ;
			}
			else if(servoIndexInitialization % 3 == 2)
			{
				//get x from angles
				legs.leg[servoIndexInitialization / NB_SERVOS_PER_LEG].targetPositionZ = DrvServoGetTarget(servoIndexInitialization) / 10  ;
				//leg is now enable
				legs.leg[servoIndexInitialization / NB_SERVOS_PER_LEG].enable = TRUE ;
				//leg is now initialized
				legs.leg[servoIndexInitialization / NB_SERVOS_PER_LEG].initialized = TRUE ;
			}
			
			//#define DEBUG_WITHOUT_SERVOS_ON_LEGS
			#ifndef DEBUG_WITHOUT_SERVOS_ON_LEGS
				//enable servo
				DrvServoActivate(servoIndexInitialization,TRUE);
			#endif
		}
		else
		{
			//initialization done
			legs.isInitialized = TRUE;
		}
		//prepare for next servo
		servoIndexInitialization++;
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
	legs.leg[ E_LEG_F_L ].coxaAngle->currentPosition		= LEG_COCYX_U_L_MID ;
	legs.leg[ E_LEG_F_L ].coxaAngle->targetPosition		= LEG_COCYX_U_L_MID ;
	legs.leg[ E_LEG_F_L ].coxaAngle->offset			= LEG_COCYX_U_L_OFT ;
	legs.leg[ E_LEG_F_L ].coxaAngle->mid			= LEG_COCYX_U_L_MID ;
	legs.leg[ E_LEG_F_L ].coxaAngle->min			= LEG_COCYX_U_L_MIN ;
	legs.leg[ E_LEG_F_L ].coxaAngle->max			= LEG_COCYX_U_L_MAX ;
	legs.leg[ E_LEG_F_L ].femurAngle->currentPosition		= LEG_FEMUR_U_L_MAX ;
	legs.leg[ E_LEG_F_L ].femurAngle->targetPosition		= LEG_FEMUR_U_L_MAX ;
	legs.leg[ E_LEG_F_L ].femurAngle->offset			= LEG_FEMUR_U_L_OFT ;
	legs.leg[ E_LEG_F_L ].femurAngle->mid			= LEG_FEMUR_U_L_MID ;
	legs.leg[ E_LEG_F_L ].femurAngle->min			= LEG_FEMUR_U_L_MIN ;
	legs.leg[ E_LEG_F_L ].femurAngle->max			= LEG_FEMUR_U_L_MAX ;
	legs.leg[ E_LEG_F_L ].tibiaAngle->currentPosition		= LEG_TIBIA_U_L_MAX ;
	legs.leg[ E_LEG_F_L ].tibiaAngle->targetPosition		= LEG_TIBIA_U_L_MAX ;
	legs.leg[ E_LEG_F_L ].tibiaAngle->offset			= LEG_TIBIA_U_L_OFT ;
	legs.leg[ E_LEG_F_L ].tibiaAngle->mid			= LEG_TIBIA_U_L_MID ;
	legs.leg[ E_LEG_F_L ].tibiaAngle->min			= LEG_TIBIA_U_L_MIN ;
	legs.leg[ E_LEG_F_L ].tibiaAngle->max			= LEG_TIBIA_U_L_MAX ;
	
	legs.leg[ E_LEG_M_L ].coxaAngle->currentPosition		= LEG_COCYX_M_L_MID ;
	legs.leg[ E_LEG_M_L ].coxaAngle->targetPosition		= LEG_COCYX_M_L_MID ;
	legs.leg[ E_LEG_M_L ].coxaAngle->offset			= LEG_COCYX_M_L_OFT ;
	legs.leg[ E_LEG_M_L ].coxaAngle->mid			= LEG_COCYX_M_L_MID ;
	legs.leg[ E_LEG_M_L ].coxaAngle->min			= LEG_COCYX_M_L_MIN ;
	legs.leg[ E_LEG_M_L ].coxaAngle->max			= LEG_COCYX_M_L_MAX ;
	legs.leg[ E_LEG_M_L ].femurAngle->currentPosition		= LEG_FEMUR_M_L_MAX ;
	legs.leg[ E_LEG_M_L ].femurAngle->targetPosition		= LEG_FEMUR_M_L_MAX ;
	legs.leg[ E_LEG_M_L ].femurAngle->offset			= LEG_FEMUR_M_L_OFT ;
	legs.leg[ E_LEG_M_L ].femurAngle->mid			= LEG_FEMUR_M_L_MID ;
	legs.leg[ E_LEG_M_L ].femurAngle->min			= LEG_FEMUR_M_L_MIN ;
	legs.leg[ E_LEG_M_L ].femurAngle->max			= LEG_FEMUR_M_L_MAX ;
	legs.leg[ E_LEG_M_L ].tibiaAngle->currentPosition		= LEG_TIBIA_M_L_MAX ;
	legs.leg[ E_LEG_M_L ].tibiaAngle->targetPosition		= LEG_TIBIA_M_L_MAX ;
	legs.leg[ E_LEG_M_L ].tibiaAngle->offset			= LEG_TIBIA_M_L_OFT ;
	legs.leg[ E_LEG_M_L ].tibiaAngle->mid			= LEG_TIBIA_M_L_MID ;
	legs.leg[ E_LEG_M_L ].tibiaAngle->min			= LEG_TIBIA_M_L_MIN ;
	legs.leg[ E_LEG_M_L ].tibiaAngle->max			= LEG_TIBIA_M_L_MAX ;
	
	legs.leg[ E_LEG_R_L ].coxaAngle->currentPosition		= LEG_COCYX_B_L_MID ;
	legs.leg[ E_LEG_R_L ].coxaAngle->targetPosition		= LEG_COCYX_B_L_MID ;
	legs.leg[ E_LEG_R_L ].coxaAngle->offset			= LEG_COCYX_B_L_OFT ;
	legs.leg[ E_LEG_R_L ].coxaAngle->mid			= LEG_COCYX_B_L_MID ;
	legs.leg[ E_LEG_R_L ].coxaAngle->min			= LEG_COCYX_B_L_MIN ;
	legs.leg[ E_LEG_R_L ].coxaAngle->max			= LEG_COCYX_B_L_MAX ;
	legs.leg[ E_LEG_R_L ].femurAngle->currentPosition		= LEG_FEMUR_B_L_MAX ;
	legs.leg[ E_LEG_R_L ].femurAngle->targetPosition		= LEG_FEMUR_B_L_MAX ;
	legs.leg[ E_LEG_R_L ].femurAngle->offset			= LEG_FEMUR_B_L_OFT ;
	legs.leg[ E_LEG_R_L ].femurAngle->mid			= LEG_FEMUR_B_L_MID ;
	legs.leg[ E_LEG_R_L ].femurAngle->min			= LEG_FEMUR_B_L_MIN ;
	legs.leg[ E_LEG_R_L ].femurAngle->max			= LEG_FEMUR_B_L_MAX ;
	legs.leg[ E_LEG_R_L ].tibiaAngle->currentPosition		= LEG_TIBIA_B_L_MAX ;
	legs.leg[ E_LEG_R_L ].tibiaAngle->targetPosition		= LEG_TIBIA_B_L_MAX ;
	legs.leg[ E_LEG_R_L ].tibiaAngle->offset			= LEG_TIBIA_B_L_OFT ;
	legs.leg[ E_LEG_R_L ].tibiaAngle->mid			= LEG_TIBIA_B_L_MID ;
	legs.leg[ E_LEG_R_L ].tibiaAngle->min			= LEG_TIBIA_B_L_MIN ;
	legs.leg[ E_LEG_R_L ].tibiaAngle->max			= LEG_TIBIA_B_L_MAX ;
	
	legs.leg[ E_LEG_F_R ].coxaAngle->currentPosition		= LEG_COCYX_U_R_MID ;
	legs.leg[ E_LEG_F_R ].coxaAngle->targetPosition		= LEG_COCYX_U_R_MID ;
	legs.leg[ E_LEG_F_R ].coxaAngle->offset			= LEG_COCYX_U_R_OFT ;
	legs.leg[ E_LEG_F_R ].coxaAngle->mid			= LEG_COCYX_U_R_MID ;
	legs.leg[ E_LEG_F_R ].coxaAngle->min			= LEG_COCYX_U_R_MIN ;
	legs.leg[ E_LEG_F_R ].coxaAngle->max			= LEG_COCYX_U_R_MAX ;
	legs.leg[ E_LEG_F_R ].femurAngle->currentPosition		= LEG_FEMUR_U_R_MIN ;
	legs.leg[ E_LEG_F_R ].femurAngle->targetPosition		= LEG_FEMUR_U_R_MIN ;
	legs.leg[ E_LEG_F_R ].femurAngle->offset			= LEG_FEMUR_U_R_OFT ;
	legs.leg[ E_LEG_F_R ].femurAngle->mid			= LEG_FEMUR_U_R_MID ;
	legs.leg[ E_LEG_F_R ].femurAngle->min			= LEG_FEMUR_U_R_MIN ;
	legs.leg[ E_LEG_F_R ].femurAngle->max			= LEG_FEMUR_U_R_MAX ;
	legs.leg[ E_LEG_F_R ].tibiaAngle->currentPosition		= LEG_TIBIA_U_R_MIN ;
	legs.leg[ E_LEG_F_R ].tibiaAngle->targetPosition		= LEG_TIBIA_U_R_MIN ;
	legs.leg[ E_LEG_F_R ].tibiaAngle->offset			= LEG_TIBIA_U_R_OFT ;
	legs.leg[ E_LEG_F_R ].tibiaAngle->mid			= LEG_TIBIA_U_R_MID ;
	legs.leg[ E_LEG_F_R ].tibiaAngle->min			= LEG_TIBIA_U_R_MIN ;
	legs.leg[ E_LEG_F_R ].tibiaAngle->max			= LEG_TIBIA_U_R_MAX ;
			  
	legs.leg[ E_LEG_M_R ].coxaAngle->currentPosition		= LEG_COCYX_M_R_MID ;
	legs.leg[ E_LEG_M_R ].coxaAngle->targetPosition		= LEG_COCYX_M_R_MID ;
	legs.leg[ E_LEG_M_R ].coxaAngle->offset			= LEG_COCYX_M_R_OFT ;
	legs.leg[ E_LEG_M_R ].coxaAngle->mid			= LEG_COCYX_M_R_MID ;
	legs.leg[ E_LEG_M_R ].coxaAngle->min			= LEG_COCYX_M_R_MIN ;
	legs.leg[ E_LEG_M_R ].coxaAngle->max			= LEG_COCYX_M_R_MAX ;
	legs.leg[ E_LEG_M_R ].femurAngle->currentPosition		= LEG_FEMUR_M_R_MIN ;
	legs.leg[ E_LEG_M_R ].femurAngle->targetPosition		= LEG_FEMUR_M_R_MIN ;
	legs.leg[ E_LEG_M_R ].femurAngle->offset			= LEG_FEMUR_M_R_OFT ;
	legs.leg[ E_LEG_M_R ].femurAngle->mid			= LEG_FEMUR_M_R_MID ;
	legs.leg[ E_LEG_M_R ].femurAngle->min			= LEG_FEMUR_M_R_MIN ;
	legs.leg[ E_LEG_M_R ].femurAngle->max			= LEG_FEMUR_M_R_MAX ;
	legs.leg[ E_LEG_M_R ].tibiaAngle->currentPosition		= LEG_TIBIA_M_R_MIN ;
	legs.leg[ E_LEG_M_R ].tibiaAngle->targetPosition		= LEG_TIBIA_M_R_MIN ;
	legs.leg[ E_LEG_M_R ].tibiaAngle->offset			= LEG_TIBIA_M_R_OFT ;
	legs.leg[ E_LEG_M_R ].tibiaAngle->mid			= LEG_TIBIA_M_R_MID ;
	legs.leg[ E_LEG_M_R ].tibiaAngle->min			= LEG_TIBIA_M_R_MIN ;
	legs.leg[ E_LEG_M_R ].tibiaAngle->max			= LEG_TIBIA_M_R_MAX ;
			  
	legs.leg[ E_LEG_R_R ].coxaAngle->currentPosition		= LEG_COCYX_B_R_MID ;
	legs.leg[ E_LEG_R_R ].coxaAngle->targetPosition		= LEG_COCYX_B_R_MID ;
	legs.leg[ E_LEG_R_R ].coxaAngle->offset			= LEG_COCYX_B_R_OFT ;
	legs.leg[ E_LEG_R_R ].coxaAngle->mid			= LEG_COCYX_B_R_MID ;
	legs.leg[ E_LEG_R_R ].coxaAngle->min			= LEG_COCYX_B_R_MIN ;
	legs.leg[ E_LEG_R_R ].coxaAngle->max			= LEG_COCYX_B_R_MAX ;
	legs.leg[ E_LEG_R_R ].femurAngle->currentPosition		= LEG_FEMUR_B_R_MIN ;
	legs.leg[ E_LEG_R_R ].femurAngle->targetPosition		= LEG_FEMUR_B_R_MIN ;
	legs.leg[ E_LEG_R_R ].femurAngle->offset			= LEG_FEMUR_B_R_OFT ;
	legs.leg[ E_LEG_R_R ].femurAngle->mid			= LEG_FEMUR_B_R_MID ;
	legs.leg[ E_LEG_R_R ].femurAngle->min			= LEG_FEMUR_B_R_MIN ;
	legs.leg[ E_LEG_R_R ].femurAngle->max			= LEG_FEMUR_B_R_MAX ;
	legs.leg[ E_LEG_R_R ].tibiaAngle->currentPosition		= LEG_TIBIA_B_R_MIN ;
	legs.leg[ E_LEG_R_R ].tibiaAngle->targetPosition		= LEG_TIBIA_B_R_MIN ;
	legs.leg[ E_LEG_R_R ].tibiaAngle->offset			= LEG_TIBIA_B_R_OFT ;
	legs.leg[ E_LEG_R_R ].tibiaAngle->mid			= LEG_TIBIA_B_R_MID ;
	legs.leg[ E_LEG_R_R ].tibiaAngle->min			= LEG_TIBIA_B_R_MIN ;
	legs.leg[ E_LEG_R_R ].tibiaAngle->max			= LEG_TIBIA_B_R_MAX ;
	return TRUE;
}