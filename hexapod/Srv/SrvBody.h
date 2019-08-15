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

////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC ENUM///////////////////////////////////////////////

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

#endif //SRVBODY_H_