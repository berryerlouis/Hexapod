/*
 * SrvComm.h
 *
 * Created: 31/05/2012 16:04:46
 *  Author: berryer
 */ 


#ifndef SRVIHM_H_
#define SRVIHM_H_

#include "Conf/ConfHard.h"
#include "Tools/tools_typedefs.h"

#include "Drv/DrvLed.h"


////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvIhmInit ( void ) ;
//Fonction de dispatching d'evenements
void SrvIhmUpdate (void) ;

//heartbeat
void SrvIhmHeartbeat ( void ) ;
void SrvIhmHeartbeatCalm ( void );
void SrvIhmHeartbeatStress ( void );
//platform init done
void SrvIhmPlatformInitDone ( void ) ;
//platform Set Left LED Time On
void SrvIhmPlatformLeftLedTimeOn ( ELedState state, Int16U delayMs );
//platform Set Right LED Time On
void SrvIhmPlatformRightLedTimeOn ( ELedState state, Int16U delayMs );
//platform Set Left LED
void SrvIhmPlatformLeftLed ( ELedState state );
//platform Set Right LED
void SrvIhmPlatformRightLed ( ELedState state );
#endif //SRVIHM_H_