/*
 * SrvBody.h
 *
 * Created: 27/07/2017 16:04:46
 *  Author: berryer
 */ 


#ifndef SRVBODY_H_
#define SRVBODY_H_

#include "Conf/ConfHard.h"
#include "Tools/tools_typedefs.h"

#include "Drv/DrvLeg.h"
#include "Cmps/CmpMPU9150.h"
#include "SrvBattery.h"
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
	SLeg *legs;
	SBatteryInfo *battery;
	SMpu9150Acc acc;
	SMpu9150Gyr gyr;
	SMpu9150Tmp tmp;
}body;
////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvBodyInit ( void ) ;
//Fonction de dispatching d'evenements
void SrvBodyUpdate (void) ;

#endif //SRVBODY_H_