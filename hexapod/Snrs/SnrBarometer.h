/*
 * SnrBarometer.h
 *
 * Created: 06/07/2012 17:29:08
 *  Author: berryer
 */ 


#ifndef CMPBAROMETER_H
#define CMPBAROMETER_H

#include "Conf/ConfHard.h"
#include "Drv/DrvTick.h"
#include "Drv/DrvEeprom.h"

////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////


////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
//define weather
typedef enum
{
	WEATHER_SUNNY,
	WEATHER_CLOUDY,
	WEATHER_RAIN,
	WEATHER_NONE
}E_Weather;

typedef struct
{
	Int16U ut;
	Int32U up;
}BarometerRawData;

typedef struct
{
	BarometerRawData	rawData;
	float				altitude;
	Int32U				pressure;
	Int32U				seaLevelPressure;
	Int16S				temperature;
	E_Weather			weather;
}BarometerData;


typedef Boolean (*CmpBarometerPtrFctInit)						( float altitude );
typedef Boolean (*CmpBarometerPtrFctUpdate)						( void );

typedef struct
{
	const CmpBarometerPtrFctInit init;		
	const CmpBarometerPtrFctUpdate update;
}BarometerFunctions;

typedef struct
{
	BarometerFunctions *functions;
	BarometerData *data;
}Barometer;

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////


Boolean SnrBarometerInit				( float altitude );
Boolean SnrBarometerUpdate				( void );
float   SnrBarometerGetTemperature		( void );
Int32S  SnrBarometerGetPressure			( void );
Int16U  SnrBarometerUpdateAltitude		( void );
E_Weather SnrBarometerUpdateWeather		( void );
BarometerData* SnrBarometerGetStruct	( void );

#endif //CMPBAROMETER_H