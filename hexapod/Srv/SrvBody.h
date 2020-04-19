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
#include "SrvUltrason.h"
#include "SrvBodyMove.h"
#include "SrvWalk.h"

////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC ENUM///////////////////////////////////////////////

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef struct SBody 
{
	SLegs *legs;
	SBatteryInfo *battery;
	SImuSimple* imu;
	SUltrason* us;
	SBodyMove *move;
	SWalk *walk;
	Boolean initialized;
}SBody;
////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvBodyInit ( void ) ;
//Fonction de dispatching d'evenements
void SrvBodyUpdate (void) ;

#endif //SRVBODY_H_