/*
 * SrvHeartbeat.h
 *
 * Created: 06/07/2012 16:35:15
 *  Author: berryer
 */ 


#ifndef SRVHEARBEAT_H_
#define SRVHEARBEAT_H_

#include "Conf/ConfHard.h"
#include "Tools/tools_typedefs.h"


//Init du hearbeat
Boolean SrvHeartbeatInit( void ) ;

//dispatcher d'evenements
void SrvHeartbeatUpdate (void) ;

void SrvHeartbeatSetPeriod ( uint8_t period );
void SrvHeartbeatStress ( void );
void SrvHeartbeatCalm ( void );

#endif /* SRVHEARBEAT_H_ */