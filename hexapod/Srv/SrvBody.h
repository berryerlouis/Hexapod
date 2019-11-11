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
#include "SrvBattery.h"
#include "SrvImuSimple.h"

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
typedef struct SBody 
{
	SLeg *legs;
	SBatteryInfo *battery;
	SImuSimple* imu;
}SBody;
////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvBodyInit ( void ) ;
//Fonction de dispatching d'evenements
void SrvBodyUpdate (void) ;

#endif //SRVBODY_H_