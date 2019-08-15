/*
 * SrvHeartbeat.h
 *
 * Created: 06/07/2012 16:35:15
 *  Author: berryer
 */ 


#ifndef SRVHEARBEAT_H_
#define SRVHEARBEAT_H_

#include "Conf/conf_hard.h"
#include "Tools/tools_typedefs.h"
#include "Drv/DrvEvent.h"


//Init du hearbeat
void SrvHeartbeatInit( void ) ;

//dispatcher d'evenements
void SrvHeartbeatDispatcher (Event_t in_event) ;


#endif /* SRVHEARBEAT_H_ */