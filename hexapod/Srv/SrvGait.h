/*
 * SrvGait.h
 *
 * Created: 01/05/2023 16:04:46
 *  Author: berryer
 */ 


#ifndef SRVGAIT_H_
#define SRVGAIT_H_

#include "Conf/ConfHard.h"
#include "Tools/tools_typedefs.h"


////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC ENUM///////////////////////////////////////////////


////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvGaitInit ( void ) ;
//Fonction de dispatching d'evenements
void SrvGaitUpdate (void) ;

#endif //SRVGAIT_H_