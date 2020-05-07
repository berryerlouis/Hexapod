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
#define IMU_THRESHOLD_ANGLE 50		//up to 3600
////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef void (*cbSrvImuReachThreshold) (Int16S roll, Int16S pitch);
typedef struct  
{
	Boolean enable;
	SMpu9150Acc acc;
	SMpu9150Gyr gyr;
	SMpu9150Tmp tmp; 
	SMpu9150GCmps cmps;
	Int8S accthreshold;
	Int16S roll;
	Int16S pitch;
	Int16S yaw;
	cbSrvImuReachThreshold cbReachThreshold;
}SImuSimple;
////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////


//Fonction d'initialisation
Boolean SrvImuSimpleInit ( void ) ;
//Fonction de dispatching d'evenements
void SrvImuSimpleUpdate (void) ;
//Get the callback
void SrvImuSimpleSetCallback (cbSrvImuReachThreshold cb) ;
//Get the sensor pointer
SImuSimple* SrvImuSimpleGetSensor (void) ;
// activate the sensor
Boolean SrvImuSimpleActivate( Boolean enable) ;
// set threshold
Boolean SrvImuSimpleSetThreshold( uint8_t accthreshold ) ;
// get threshold
uint8_t SrvImuSimpleGetThreshold( void ) ;
// get roll pitch yaw
void SrvImuSimpleGetRollPitchYaw( Int16S *roll, Int16S *pitch, Int16S *yaw );
// get roll
Int16S SrvImuSimpleGetRoll( void ) ;
// get pitch
Int16S SrvImuSimpleGetPitch( void ) ;
// get yaw
Int16S SrvImuSimpleGetYaw( void ) ;
#endif //SRVIMUSIMPLE_H_