/*
 * SrvImu.h
 *
 * Created: 06/07/2012 17:29:08
 *  Author: berryer
 */ 


#ifndef CMPGYROSCOPE_H
#define CMPGYROSCOPE_H

#include "Conf/ConfHard.h"
#include "Drv/DrvTick.h"
#include "Drv/DrvEeprom.h"

////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////
typedef enum
{
	GYR_250DPS,
	GYR_500DPS,
	GYR_2000DPS
}E_Gyr_Setup;

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef Axis GyroscopeRawData;

typedef AxisFloat GyroscopeNoise;

typedef struct
{
	AxisFloat axis;
	Boolean status;
}GyroscopeCalibrationData;

typedef struct
{
	GyroscopeCalibrationData calibrationData;
	GyroscopeRawData rawData;
	GyroscopeNoise noise;
	float degreePerSeconds;
	float roulis;
	float tangage;
	float lacet;
}GyroscopeData;


typedef Boolean (*CmpGyroscopePtrFctInit)		( E_Gyr_Setup setup);
typedef Boolean (*CmpGyroscopePtrFctReadData)	( void );
typedef Int8S (*CmpGyroscopePtrFctGetTemperature)	( void );
typedef struct
{
	const CmpGyroscopePtrFctInit init;		
	const CmpGyroscopePtrFctReadData readData;
	const CmpGyroscopePtrFctGetTemperature readTemperature;
}GyroscopeFunctions;

typedef struct
{
	GyroscopeFunctions *functions;
	GyroscopeData *data;
}Gyroscope;

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////


Boolean SnrGyroscopeInit				( E_Gyr_Setup setup );
Boolean SnrGyroscopeGetRotation     	( void );
Int8S   SnrGyroscopeGetTemperature		( void );
Boolean SnrGyroscopeIsCalibrate			( void );
Boolean SnrGyroscopeSetCalibration		( void );
Boolean	SnrGyroscopeSaveCalibration		( void );
GyroscopeData* SnrGyroscopeGetStruct( void );

#endif //CMPGYROSCOPE_H