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

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvUltrasonInit ( void ) ;
//Fonction de dispatching d'evenements
void SrvUltrasonUpdate (void) ;
// set threshold
Boolean SrvUltrasonSetThreshold( uint8_t threshold );
// get threshold
uint8_t SrvUltrasonGetThreshold( void );
//get distance
uint16_t SrvUltrasonGetDistance (E_US us) ;

#endif //SRVULTRASON_H_