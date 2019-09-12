/*
 * SrvBody.h
 *
 * Created: 27/07/2017 16:04:46
 *  Author: berryer
 */ 


#ifndef SRVBODY_H_
#define SRVBODY_H_

#include "../Conf/ConfHard.h"
#include "../Tools/tools_typedefs.h"

#include "../Drv/DrvLeg.h"
#include "SrvWalk.h"

////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC ENUM///////////////////////////////////////////////
typedef enum
{
	E_BODY_STAR,
	E_BODY_STRAIGHT,
	E_BODY_UNKNOWNN,
} E_BODY_POSITION;
////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
struct SBody 
{
	S_Leg *legs[NB_LEGS];
	
}Body;
////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvBodyInit ( void ) ;
//Fonction de dispatching d'evenements
void SrvBodyUpdate (void) ;
Boolean SrvBodySetPosition( E_BODY_POSITION position, uint16_t delay);
Boolean SrvBodySetWalk( E_WALK walk, uint16_t delay );
Boolean SrvBodySetGait( E_GAIT gait, uint16_t delay );

#endif //SRVBODY_H_