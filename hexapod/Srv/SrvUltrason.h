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
#define US_THRESHOLD_DISTANCE	45U		// 45 cm

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
//struct of legs, 
typedef struct
{
	uint16_t usthreshold;
	uint16_t distance [E_NB_USS];
}SUltrason;

typedef void (*SrvUltrasonCallback) (E_US us, uint16_t distance);

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvUltrasonInit ( void ) ;
//Fonction de dispatching d'evenements
void SrvUltrasonUpdate (void) ;

SUltrason* SrvUltrasonGetStruct( void );

// set threshold
Boolean SrvUltrasonSetThreshold( uint8_t threshold );
// get threshold
uint8_t SrvUltrasonGetThreshold( void );
//get distance
uint16_t SrvUltrasonGetDistance (E_US us) ;

void SrvUltrasonSetCallbackThreshold ( SrvUltrasonCallback cb ) ;

#endif //SRVULTRASON_H_