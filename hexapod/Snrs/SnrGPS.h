/*
 * SnrGPS.h
 *
 * Created: 06/07/2012 17:29:08
 *  Author: berryer
 */ 


#ifndef CMPGPS_H
#define CMPGPS_H

#include "Conf/ConfHard.h"
#include "Drv/DrvTick.h"
#include "Drv/DrvEeprom.h"

////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////
typedef enum
{
	GPS_POSITION_UNAVAILABLE,
	GPS_SPS_MODE,
	GPS_GPS_MODE,
	GPS_PPS_MODE,
	GPS_INT_RTK_MODE,
	GPS_FLOAT_RTK_MODE,
	GPS_ESTIMATED_MODE,
	GPS_MANUAL_INPUT_MODE,
	GPS_SIMULATOR_MODE,
	GPS_MODE_NONE
}E_GpsQualityIndicator;

typedef enum
{
	GPS_NORTH,
	GPS_SOUTH,
	GPS_EAST,
	GPS_WEST,
	GPS_NONE
}E_GpsHeadingIndicator;



////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef struct
{
	Int8U hour;
	Int8U minute;
	Int8U second;
	Int16U millisecond;
}S_GpsUTCTime;

typedef struct
{
	S_GpsUTCTime time;
	float latitude;
	E_GpsHeadingIndicator northSouth;
	float longitude;
	E_GpsHeadingIndicator eastWest;
	E_GpsQualityIndicator quality;
	Int8U satellites;
	float HDOP;
	float altitude;
	Int16U stationId;
	float speed;
	float heading;
	Int32U lastTimeRcvFrame;
}GpsData;


typedef Boolean (*CmpGpsPtrFctInit)		( void );
typedef Boolean (*CmpGpsPtrFctReadData)	( void );

typedef struct
{
	const CmpGpsPtrFctInit init;		
	const CmpGpsPtrFctReadData readData;
}GpsFunctions;

typedef struct
{
	GpsFunctions *functions;
	GpsData *data;
}Gps;

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////


Boolean SnrGpsInit			( void );
Boolean SnrGpsDecodeGPSFrame( void );
GpsData* SnrGpsGetStruct	( void );

#endif //CMPGPS_H