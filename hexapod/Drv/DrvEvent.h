/*
 * drv_event.h
 *
 * Created: 14/11/2011 15:18:48
 *  Author: berryer
 */ 


#ifndef DRV_EVENT_H_
#define DRV_EVENT_H_


#include "Tools/tools_typedefs.h"
////////////////////////////////////////////PUBLIC ENUMS///////////////////////////////////////////



/////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////


typedef Int16U Event_t;

// Init du Drv Event 
Boolean DrvEventInit( void ) ;
// On recupere l'evenement en cours   
Event_t DrvEventGetEvent( void ) ;
//On test l'evenement en cours 
Boolean DrvEventTestEvent( Event_t iEvent, Event_t conf_event ) ;
//On ajoute un evenement aux evenements en cours  
Boolean DrvEventAddEvent( Event_t event ) ;
//On ajoute un evenement aux evenements en cours lorsqu'on est dans une ISR
Boolean DrvEventAddEventInISR(Event_t iEvent);
#endif /* DRV_EVENT_H_ */