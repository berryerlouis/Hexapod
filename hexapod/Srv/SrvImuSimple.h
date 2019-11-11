/*
 * SrvImuSimple.h
 *
 * Created: 27/07/2017 16:04:46
 *  Author: berryer
 */ 


#ifndef SRVIMUSIMPLE_H_
#define SRVIMUSIMPLE_H_

#include "Conf/ConfHard.h"
#include "Tools/tools_typedefs.h"

#include "Cmps/CmpMPU9150.h"

////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC ENUM///////////////////////////////////////////////

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef struct  
{
	Boolean enable;
	SMpu9150Acc acc;
	SMpu9150Gyr gyr;
	SMpu9150Tmp tmp;
	uint8_t accthreshold;
}SImuSimple;
////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvImuSimpleInit ( void ) ;
//Fonction de dispatching d'evenements
void SrvImuSimpleUpdate (void) ;
//Get the sensor pointer
SImuSimple* SrvImuSimpleGetSensor (void) ;
// activate the sensor
Boolean SrvImuSimpleActivate( Boolean enable) ;
#endif //SRVIMUSIMPLE_H_