/*
 * SrvStartEngine.h
 *
 * Created: 06/07/2012 16:35:15
 *  Author: berryer
 */ 


#ifndef SRVSTARTENGINE_H_
#define SRVSTARTENGINE_H_

#include "Conf/ConfHard.h"
#include "Tools/tools_typedefs.h"

#include "Drv/DrvEvent.h"
//Init des valeurs d'allumage moteurs
void SrvStartEngineInit( void ) ;
//dispatcher d'evenements
void SrvStartEngineDispatcher (Event_t in_event) ;


#endif /* SRVSTARTENGINE_H_ */