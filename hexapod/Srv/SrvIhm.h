/*
 * SrvComm.h
 *
 * Created: 31/05/2012 16:04:46
 *  Author: berryer
 */ 


#ifndef SRVIHM_H_
#define SRVIHM_H_

#include "../Conf/ConfHard.h"
#include "../Tools/tools_typedefs.h"


////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvIhmInit ( void ) ;
//Fonction de dispatching d'evenements
void SrvIhmDispatcher (void) ;

//heartbeat
void SrvIhmHeartbeat ( void ) ;
//platform init start
void SrvIhmPlatformInitStart ( void ) ;
//platform init done
void SrvIhmPlatformInitDone ( void ) ;
//platform Disarm
void SrvIhmPlatformDisarm ( void ) ;
//platform Arm
void SrvIhmPlatformArm ( void ) ;
//platform Error
void SrvIhmPlatformError ( void ) ;
#endif //SRVIHM_H_