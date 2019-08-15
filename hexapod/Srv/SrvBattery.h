/*
 * SrvBattery.h
 *
 * Created: 27/07/2017 16:04:46
 *  Author: berryer
 */ 


#ifndef SRVBATTERY_H_
#define SRVBATTERY_H_

#include "Conf/ConfHard.h"
#include "Tools/tools_typedefs.h"


////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC ENUM///////////////////////////////////////////////
typedef enum 
{
	BATTERY_100,
	BATTERY_75,
	BATTERY_50,
	BATTERY_25,
	BATTERY_0
}EBatteryLevel;
////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvBatteryInit ( void ) ;
//Fonction de dispatching d'evenements
void SrvBatteryUpdate (void) ;

EBatteryLevel SrvBatteryGetLevel (void) ;
uint8_t SrvBatteryGetValue (void) ;

#endif //SRVBATTERY_H_