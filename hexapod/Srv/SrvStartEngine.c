/*
 * SrvStartEngine.c
 *
 * Created: 06/07/2012 16:34:50
 *  Author: berryer
 */ 

#include "SrvStartEngine.h"
#include "SrvMotor.h"



////////////////////////////////////////PRIVATE DEFINES/////////////////////////////////////////
#define LIMIT_MOTORS_START		21
#define MOTORS_START			0
#define TIMEOUT_ALLUMAGE		5
#define DISABLE_TIMEOUT_ALLUMAGE 0

////////////////////////////////////////PRIVATE VARIABLES///////////////////////////////////////
Int8U speed_moteur = 0U;
Int8U timeout_before_allumage = 0U;
Boolean allumage = FALSE;

//Init des valeurs d'allumage moteurs
void SrvStartEngineInit( void )
{
	speed_moteur = MOTORS_START;
	allumage = TRUE ;
	timeout_before_allumage = TIMEOUT_ALLUMAGE;
}	

//dispatcher d'evenements
void SrvStartEngineDispatcher (Event_t in_event) 
{
	if( DrvEventTestEvent(in_event,CONF_EVENT_TIMER_1S))
	{
		//si le timeout est ecoulé
		if((allumage == TRUE) && (timeout_before_allumage == DISABLE_TIMEOUT_ALLUMAGE))
		{
			//on fait un allumage progressif des moteurs jusqu'a une vitesse correct
			speed_moteur += 5U;
			if( speed_moteur >= LIMIT_MOTORS_START )
			{
				speed_moteur = LIMIT_MOTORS_START;
				allumage = FALSE;
			}
			SrvMotorApplyAbsoluteSpeed(speed_moteur);
		}
		//on decompte si on est en mode allumage
		else if (allumage == TRUE)
		{
			//si le timeout est active on decompte
			if(timeout_before_allumage != DISABLE_TIMEOUT_ALLUMAGE)
			{
				timeout_before_allumage--;
			}
		}
	}		
}
