/*
 * SrvDisplay.h
 *
 * Created: 27/07/2017 16:04:46
 *  Author: berryer
 */ 


#ifndef SRVDISPLAY_H_
#define SRVDISPLAY_H_

#include "Tools/tools_typedefs.h"

#include "Drv/DrvSSD1306.h"
#include "Drv/DrvBitmap.h"
#include "Drv/DrvFont.h"

////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC ENUM///////////////////////////////////////////////

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvDisplayInit ( void ) ;
//Fonction de dispatching d'evenements
void SrvDisplayUpdate (void) ;

#endif //SRVDISPLAY_H_