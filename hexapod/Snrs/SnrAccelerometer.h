/*
 * SrvImu.h
 *
 * Created: 06/07/2012 17:29:08
 *  Author: berryer
 */ 


#ifndef CMPACCELEROMETER_H
#define CMPACCELEROMETER_H

#include "Conf/ConfHard.h"
#include "Drv/DrvTick.h"
#include "Drv/DrvEeprom.h"

////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////
typedef enum
{
	ACC_1G_TO_2G,
	ACC_1G_TO_4G,
	ACC_1G_TO_8G
}E_Acc_Setup;

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef Axis AccelerometerRawData;

typedef struct
{
	Axis32 axis;
	Boolean status;
}AccelerometerCalibrationData;

typedef struct
{
	AccelerometerCalibrationData calibrationData;
	AccelerometerRawData rawData;
	Int16U gravity;
	float roulis;
	float tangage;
}AccelerometerData;


typedef Boolean (*CmpAccelerometerPtrFctInit)		( E_Acc_Setup setup );
typedef Boolean (*CmpAccelerometerPtrFctReadData)	( void );
typedef struct
{
	const CmpAccelerometerPtrFctInit init;		
	const CmpAccelerometerPtrFctReadData readData;
}AccelerometerFunctions;

typedef struct
{
	AccelerometerFunctions *functions;
	AccelerometerData *data;
}Accelerometer;

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////


Boolean SnrAccelerometerInit				( E_Acc_Setup setup);
Boolean SnrAccelerometerGetAcceleration		( void );
Boolean SnrAccelerometerIsCalibrate			( void );
Boolean SnrAccelerometerSetCalibration		( void );
Boolean	SnrAccelerometerSaveCalibration		( void );
AccelerometerData* SnrAccelerometerGetStruct( void );
#endif //CMPACCELEROMETER_H