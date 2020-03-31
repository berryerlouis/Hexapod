/*
 * SrvBody.h
 *
 * Created: 27/07/2017 16:04:46
 *  Author: berryer
 */ 


#ifndef SRVBODY_H_
#define SRVBODY_H_

#include "Conf/ConfHard.h"
#include "Tools/tools_typedefs.h"

#include "Drv/DrvLeg.h"
#include "SrvBattery.h"
#include "SrvImuSimple.h"

////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC ENUM///////////////////////////////////////////////
typedef enum
{
	E_BODY_STOP,
	E_BODY_STAR,
	E_BODY_STRAIGHT,
	E_BODY_UNKNOWNN,
} EBodyBehavior;

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef struct SBody 
{
	SLegs *legs;
	SBatteryInfo *battery;
	SImuSimple* imu;
	EBodyBehavior behavior;
	uint8_t elevation;
	int8_t x;
	int8_t y;
}SBody;
////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvBodyInit ( void ) ;
//Fonction de dispatching d'evenements
void SrvBodyUpdate (void) ;
//Set Position
Boolean SrvBodySetBehavior ( EBodyBehavior pos, uint16_t delay );
//Get Position
EBodyBehavior SrvBodyGetBehavior ( void );
//Set Elevation
Boolean SrvBodySetElevation ( uint8_t elev, uint16_t delay );
//Get Elevation
uint8_t SrvBodyGetElevation ( void );
//Set Position
Boolean SrvBodySetPosition ( int8_t x, int8_t y );

#endif //SRVBODY_H_