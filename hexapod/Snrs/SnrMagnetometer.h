/*
 * SrvImu.h
 *
 * Created: 06/07/2012 17:29:08
 *  Author: berryer
 */ 


#ifndef CMPMAGNETOMETER_H
#define CMPMAGNETOMETER_H

#include "Conf/ConfHard.h"
#include "Drv/DrvTick.h"
#include "Drv/DrvEeprom.h"

////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////
//http://www.magnetic-declination.com/
//http://www.ngdc.noaa.gov/geomag-web/
#define LOCAL_MAGNETIC_DECLINAISON		1.39F

typedef enum
{
	MAG_0_88GA,
	MAG_1_3GA,
	MAG_1_9GA,
	MAG_2_5GA,
	MAG_4_0GA,
	MAG_4_7GA,
	MAG_5_6GA,
	MAG_8_1GA
}E_Mag_Setup;



////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef Axis MagnetometerRawData;

typedef AxisFloat MagnetometerGain;

typedef struct
{
	AxisFloat axis;
	Boolean status;
}MagnetometerCalibrationData;

typedef struct
{
	MagnetometerRawData rawData;
	MagnetometerGain gainData;
	MagnetometerCalibrationData calibrationData;
	Int16S heading;
	float declination;
}MagnetometerData;


typedef Boolean (*CmpMagnetometerPtrFctInit)		( E_Mag_Setup setup );
typedef Boolean (*CmpMagnetometerPtrFctReadData)	( void );
typedef struct
{
	const CmpMagnetometerPtrFctInit init;		
	const CmpMagnetometerPtrFctReadData readData;
}MagnetometerFunctions;

typedef struct
{
	MagnetometerFunctions *functions;
	MagnetometerData *data;
}Magnetometer;

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////


Boolean SnrMagnetometerInit					( E_Mag_Setup setup );
Boolean SnrMagnetometerGetScaleData     		( void );
Boolean SnrMagnetometerIsCalibrate			( void );
Boolean SnrMagnetometerSetCalibration		( void );
Boolean	SnrMagnetometerSaveCalibration		( void );
MagnetometerData* SnrMagnetometerGetStruct	( void );

#endif //CMPMAGNETOMETER_H