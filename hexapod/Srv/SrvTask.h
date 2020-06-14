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
#include "SrvDetection.h"
#include "SrvBodyMove.h"
#include "SrvWalk.h"
#include "SrvFeeling.h"
#include "Srv/SrvHead.h"

////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC ENUM///////////////////////////////////////////////

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////
typedef void (*CBNotifyTask)(void);
////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef struct
{
	Int8U id;
	Boolean enable;
	CBNotifyTask notify;
	Int32U updateTime;
	Int32U timeout;
}STask;
typedef struct 
{
	SLegs *legs;
	SHead *head;
	SWalk *walk;
	SBatteryInfo *battery;
	SImuSimple* imu;
	SDetection* detection;
	SBodyMove *move;
	SFeeling *feeling;
	Boolean initialized;
	STask *tasks;
	Int8U nbTasks;
}STaskPublic;
////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvTasknit ( void ) ;
//Fonction de dispatching d'evenements
void SrvTaskUpdate (void) ;

void SrvTaskEnable (Int8U taskId);
void SrvTaskDisable (Int8U taskId);
#endif //SRVBODY_H_