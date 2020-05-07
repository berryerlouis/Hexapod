/*
 * SrvBattery.h
 *
 * Created: 27/07/2017 16:04:46
 *  Author: berryer
 */ 


#ifndef SRVULTRASON_H_
#define SRVULTRASON_H_

#include "Conf/ConfHard.h"
#include "Tools/tools_typedefs.h"
#include "Cmps/CmpSRF04.h"


////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC ENUM///////////////////////////////////////////////

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////
#define US_THRESHOLD_DISTANCE	20U		// cm

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
//struct of legs, 
typedef struct
{
	Int16U usthreshold;
	Int16U distance [E_NB_USS];
}SUltrason;

typedef void (*SrvUltrasonCallback) (E_US us, uint16_t distance);

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvUltrasonInit ( void ) ;
//Fonction de dispatching d'evenements
void SrvUltrasonUpdate (void) ;

SUltrason* SrvUltrasonGetStruct( void );

// set threshold
Boolean SrvUltrasonSetThreshold( Int16U threshold );
// get threshold
Int16U SrvUltrasonGetThreshold( void );
//get distance
Int16U SrvUltrasonGetDistance (E_US us) ;

Boolean SrvUltrasonReachThreshold( E_US us );
void SrvUltrasonSetCallbackThreshold ( SrvUltrasonCallback cb ) ;

#endif //SRVULTRASON_H_