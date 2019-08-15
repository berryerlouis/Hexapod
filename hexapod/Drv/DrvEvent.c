/*
 * drv_event.c
 *
 * Created: 28/06/2011 15:52:35
 *  Author: berryer
 */ 
#include "Conf/ConfHard.h"

#include "Drv/DrvEvent.h"

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
static volatile Event_t event_flags = 0U;
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

  
/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
/************************************************************************/
/*Init du Drv Event                                                     */
/************************************************************************/
Boolean DrvEventInit( void )
{
	event_flags = 0U;
	return TRUE;
}

/************************************************************************/
/*On recupere l'evenement en cours                                      */
/************************************************************************/
inline Event_t DrvEventGetEvent( void )
{
	Event_t temp_added_flags = 0U;
	ATOMIC
	(
		//on kill
		temp_added_flags = event_flags ;
	);
	event_flags = 0U;
	return temp_added_flags;
}

/************************************************************************/
/*On test l'evenement en cours                                          */
/************************************************************************/
inline Boolean DrvEventTestEvent( Event_t iEvent, Event_t conf_event ) 
{
	Boolean return_value = FALSE;
	ATOMIC
	(
		if(( iEvent & conf_event) > 0U ) return_value = TRUE;
	);
	return return_value;
}

/************************************************************************/
/*On ajoute un evenement aux evenements en cours                        */
/************************************************************************/
inline Boolean DrvEventAddEvent(Event_t iEvent)
{
	ATOMIC
	(
		event_flags |= iEvent ;
	);
	
	return TRUE;
}
inline Boolean DrvEventAddEventInISR(Event_t iEvent)
{
	event_flags |= iEvent ;	
	return TRUE;
}