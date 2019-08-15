/*
 * Cmpvenus638lpxData.c
 *
 * Created: 03/07/2012 13:48:49
 *  Author: berryer
 */
/////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
#include "CmpVENUS638LPX.h"
 
#include "Drv/DrvUart.h"
#include "Drv/DrvTick.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

/////////////////////////////////////////PRIVATE STRUCTIURES///////////////////////////////////////
//enum used for parsing the incomming frame
typedef enum {
	GPS_STATE_WAIT_PREFIX = 0U,
	GPS_STATE_WAIT_PREFIX_1,
	GPS_STATE_WAIT_PREFIX_2,
	GPS_STATE_WAIT_SUFFIX,
	GPS_STATE_WAIT_SUFFIX_1,
} E_GPSState;
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

static Boolean CmpVENUS638LPXDecodeGPSFrameGGA( void );
static Boolean CmpVENUS638LPXDecodeGPSFrameRMC( void );
static Boolean CmpVENUS638LPXDecodeGPSFrameVTG( void );

static Boolean CmpVenux638LPXInit( void );
static Boolean CmpVenux638LPXReadData( void );

//definition de la structure des fonctions du ultrasonic pour le composant SRF04
GpsFunctions venus638lpx =
{
	CmpVenux638LPXInit,
	CmpVenux638LPXReadData,
};

//definition de la structure des data ultrasonic pour le composant SRF04
GpsData venus638lpxData;
////////////////////////////////////////ISR FUNCTIONS////////////////////////////////////////////

/////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////

//preamble of GPS frames
Char* prefixGP		= "$GP";
Char* prefixGGA		= "GGA";
Char* prefixRMC		= "RMC";
Char* prefixVTG		= "VTG";
Char* suffixGPS		= "\r\n";

//buffer def
struct
{
	Int8U buffer[255U];
	Int8U index;
}gpsBuffer;

/************************************************************************/
/*init de la communication                                              */
/************************************************************************/
static Boolean CmpVenux638LPXInit ( void )
{
	//uart init
	return DrvUartInit( E_UART_2, UART_SPEED_115200 );
}

static Boolean CmpVenux638LPXReadData( void )
{
	Boolean oSuccess = FALSE;
	static E_GPSState status = GPS_STATE_WAIT_PREFIX;

	//si on recoit des messages regulierement (< 2 sec)
	if(( DrvTickGetTimeUs() - venus638lpxData.lastTimeRcvFrame) > 2000000)
	{
		venus638lpxData.quality = GPS_POSITION_UNAVAILABLE;
		venus638lpxData.satellites = 0U;
	}

	Int16U nbDataAvailable = DrvUartDataAvailable(E_UART_2);

	for(Int16U loop = 0U; loop < nbDataAvailable ; loop++)
	{
		//get the data
		Int8U data = DrvUartReadData( E_UART_2 );
		switch ((Int8U)status)
		{
			case GPS_STATE_WAIT_PREFIX:
			if(data == prefixGP[ 0U ])
			{
				gpsBuffer.index = 0U;
				gpsBuffer.buffer[gpsBuffer.index++] = data;
				status    = GPS_STATE_WAIT_PREFIX_1;
			}
			break;
			case GPS_STATE_WAIT_PREFIX_1:
			if(data == prefixGP[ 1U ])
			{
				gpsBuffer.buffer[gpsBuffer.index++] = data;
				status    = GPS_STATE_WAIT_PREFIX_2;
			}
			else
			{
				status    = GPS_STATE_WAIT_PREFIX;
			}
			break;
			case GPS_STATE_WAIT_PREFIX_2:
			if(data == prefixGP[ 2U ])
			{
				gpsBuffer.buffer[gpsBuffer.index++] = data;
				status    = GPS_STATE_WAIT_SUFFIX;
			}
			else
			{
				status    = GPS_STATE_WAIT_PREFIX;
			}
			break;
			case GPS_STATE_WAIT_SUFFIX:
			gpsBuffer.buffer[gpsBuffer.index++] = data;
			if(data == suffixGPS[ 0U ])
			{
				status    = GPS_STATE_WAIT_SUFFIX_1;
			}
			//forbidden char
			else if( (data == prefixGP[ 0U ]) || (data == suffixGPS[ 1U ]))
			{
				status    = GPS_STATE_WAIT_PREFIX;
			}
			break;
			case GPS_STATE_WAIT_SUFFIX_1:
			if(data == suffixGPS[ 1U ])
			{
				gpsBuffer.buffer[gpsBuffer.index++] = data;
				status    = GPS_STATE_WAIT_PREFIX;
				
				
				if( 0U == memcmp(&gpsBuffer.buffer[3U],prefixGGA,3U) )
				{
					oSuccess = CmpVENUS638LPXDecodeGPSFrameGGA();
				}
				else if ( 0U == memcmp(&gpsBuffer.buffer[3U],prefixRMC,3U) )
				{
					oSuccess = CmpVENUS638LPXDecodeGPSFrameRMC();
				}
				else if ( 0U == memcmp(&gpsBuffer.buffer[3U],prefixVTG,3U) )
				{
					oSuccess = CmpVENUS638LPXDecodeGPSFrameVTG();
				}
				
				if(oSuccess == TRUE)
				{
					venus638lpxData.lastTimeRcvFrame = DrvTickGetTimeUs();
				}
			}
			else
			{
				status    = GPS_STATE_WAIT_PREFIX;
			}
			break;
		}
	}
	return oSuccess;
}


////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
static Boolean CmpVENUS638LPXDecodeGPSFrameVTG( void )
{
	Boolean oSuccess = FALSE;
	Int8U indexParam = 0U;
	//start after prefixGP + prefixRMC
	Int8U indexParamStart = 7U;
	Int8U indexParamEnd = 0U;
	//start after prefixGP + prefixRMC
	for(Int8U loop = 7U; loop < (gpsBuffer.index - 4U); loop++)
	{
		if( (gpsBuffer.buffer[ loop ] == ',') || (gpsBuffer.buffer[ loop ] == '*'))
		{
			//record the end
			indexParamEnd = loop;
			//wait the Speed over ground parameter
			if( indexParam == 0U )
			{
				//course over ground parameter in km/h
				
				//fixed size
				if( indexParamEnd - indexParamStart == 5U )
				{
					venus638lpxData.heading  = (float)(gpsBuffer.buffer[ indexParamStart ] - 0x30) * 100U;
					venus638lpxData.heading  += (float)(gpsBuffer.buffer[ indexParamStart + 1U ] - 0x30) * 10U;
					venus638lpxData.heading  += (float)(gpsBuffer.buffer[ indexParamStart + 2U ] - 0x30);
					venus638lpxData.heading += (float)(gpsBuffer.buffer[ indexParamStart + 4U ] - 0x30) * 0.1F;
				}
			}
			else if( indexParam == 4U )
			{
				//Speed over ground parameter in km/h
				
				//fixed size
				if( indexParamEnd - indexParamStart == 5U )
				{
					venus638lpxData.speed = (float)(gpsBuffer.buffer[ indexParamStart ] - 0x30) * 100U;
					venus638lpxData.speed += (float)(gpsBuffer.buffer[ indexParamStart + 1U ] - 0x30) * 10U;
					venus638lpxData.speed += (float)(gpsBuffer.buffer[ indexParamStart + 2U ] - 0x30);
					venus638lpxData.speed += (float)(gpsBuffer.buffer[ indexParamStart + 4U ] - 0x30) * 0.1F;
					oSuccess = TRUE;
				}
			}
			indexParam++;
			//after ',' go to next param
			indexParamStart = loop + 1U;
		}
	}
	return oSuccess;
}
static Boolean CmpVENUS638LPXDecodeGPSFrameGGA( void )
{
	Boolean oSuccess = FALSE;
	Int8U indexParam = 0U;
	//start after prefixGP + prefixGGA
	Int8U indexParamStart = 7U;
	Int8U indexParamEnd = 0U;
	//start after prefixGP + prefixGGA
	for(Int8U loop = 7U; loop < (gpsBuffer.index - 4U); loop++)
	{
		if((gpsBuffer.buffer[ loop ] == ',') || (gpsBuffer.buffer[ loop ] == '*'))
		{
			//record the end
			indexParamEnd = loop;
			if( indexParam == 0U )
			{
				//utc time
				
				//fixed size
				if( indexParamEnd - indexParamStart == 10U )
				{
					venus638lpxData.time.hour = (Int8U)(gpsBuffer.buffer[ indexParamStart ] - 0x30) * 10U;
					venus638lpxData.time.hour += (Int8U)(gpsBuffer.buffer[ indexParamStart + 1U ] - 0x30);
					
					venus638lpxData.time.minute = (Int8U)(gpsBuffer.buffer[ indexParamStart + 2U ] - 0x30) * 10U;
					venus638lpxData.time.minute += (Int8U)(gpsBuffer.buffer[ indexParamStart + 3U ] - 0x30);
					
					venus638lpxData.time.second = (Int8U)(gpsBuffer.buffer[ indexParamStart + 4U ] - 0x30) * 10U;
					venus638lpxData.time.second += (Int8U)(gpsBuffer.buffer[ indexParamStart + 5U ] - 0x30);
					
					venus638lpxData.time.millisecond = (Int8U)(gpsBuffer.buffer[ indexParamStart + 7U ] - 0x30) * 100U;
					venus638lpxData.time.millisecond += (Int8U)(gpsBuffer.buffer[ indexParamStart + 8U ] - 0x30) * 10U;
					venus638lpxData.time.millisecond += (Int8U)(gpsBuffer.buffer[ indexParamStart + 9U ] - 0x30);
					
				}
				indexParam++;
			}
			else if( indexParam == 1U )
			{
				//latitude
				
				//fixed size
				if( indexParamEnd - indexParamStart == 9U )
				{
					venus638lpxData.latitude = (float)(gpsBuffer.buffer[ indexParamStart ] - 0x30) * 1000U;
					venus638lpxData.latitude += (float)(gpsBuffer.buffer[ indexParamStart + 1U ] - 0x30) * 100U;
					venus638lpxData.latitude += (float)(gpsBuffer.buffer[ indexParamStart + 2U ] - 0x30) * 10U;
					venus638lpxData.latitude += (float)(gpsBuffer.buffer[ indexParamStart + 3U ] - 0x30);
					venus638lpxData.latitude += (float)(gpsBuffer.buffer[ indexParamStart + 5U ] - 0x30) * 0.1F;
					venus638lpxData.latitude += (float)(gpsBuffer.buffer[ indexParamStart + 6U ] - 0x30) * 0.01F;
					venus638lpxData.latitude += (float)(gpsBuffer.buffer[ indexParamStart + 7U ] - 0x30) * 0.001F;
					venus638lpxData.latitude += (float)(gpsBuffer.buffer[ indexParamStart + 8U ] - 0x30) * 0.0001F;
				}
				indexParam++;
			}
			else if( indexParam == 2U )
			{
				//north south
				//fixed size
				if( indexParamEnd - indexParamStart == 1U )
				{
					if( gpsBuffer.buffer[ indexParamStart ] == 'N' )
					{
						venus638lpxData.northSouth = (E_GpsHeadingIndicator)GPS_NORTH;
					}
					else if( gpsBuffer.buffer[ indexParamStart ] == 'S' )
					{
						venus638lpxData.northSouth = (E_GpsHeadingIndicator)GPS_SOUTH;
					}
					else
					{
						venus638lpxData.northSouth = (E_GpsHeadingIndicator)GPS_NONE;
					}
				}
				indexParam++;
			}
			else if( indexParam == 3U )
			{
				//longitude
				
				//fixed size
				if( indexParamEnd - indexParamStart == 10U )
				{
					venus638lpxData.longitude = (float)(gpsBuffer.buffer[ indexParamStart ] - 0x30) * 10000U;
					venus638lpxData.longitude += (float)(gpsBuffer.buffer[ indexParamStart + 1U ] - 0x30) * 1000U;
					venus638lpxData.longitude += (float)(gpsBuffer.buffer[ indexParamStart + 2U ] - 0x30) * 100U;
					venus638lpxData.longitude += (float)(gpsBuffer.buffer[ indexParamStart + 3U ] - 0x30) * 10U;
					venus638lpxData.longitude += (float)(gpsBuffer.buffer[ indexParamStart + 4U ] - 0x30);
					venus638lpxData.longitude += (float)(gpsBuffer.buffer[ indexParamStart + 6U ] - 0x30) * 0.1F;
					venus638lpxData.longitude += (float)(gpsBuffer.buffer[ indexParamStart + 7U ] - 0x30) * 0.01F;
					venus638lpxData.longitude += (float)(gpsBuffer.buffer[ indexParamStart + 8U ] - 0x30) * 0.001F;
					venus638lpxData.longitude += (float)(gpsBuffer.buffer[ indexParamStart + 9U ] - 0x30) * 0.0001F;
				}
				indexParam++;
			}
			else if( indexParam == 4U )
			{
				//east west
				
				//fixed size
				if( indexParamEnd - indexParamStart == 1U )
				{
					if( gpsBuffer.buffer[ indexParamStart ] == 'E' )
					{
						venus638lpxData.eastWest = (E_GpsHeadingIndicator)GPS_EAST;
					}
					else if( gpsBuffer.buffer[ indexParamStart ] == 'W' )
					{
						venus638lpxData.eastWest = (E_GpsHeadingIndicator)GPS_WEST;
					}
					else
					{
						venus638lpxData.eastWest = (E_GpsHeadingIndicator)GPS_NONE;
					}
				}
				indexParam++;
			}
			else if( indexParam == 5U )
			{
				//GPS quality
				
				//fixed size
				if( indexParamEnd - indexParamStart == 1U )
				{
					venus638lpxData.quality = (E_GpsQualityIndicator)gpsBuffer.buffer[ indexParamStart ] - 0x30U;
				}
				indexParam++;
			}
			else if( indexParam == 6U )
			{
				//satellites
				
				//fixed size
				if( indexParamEnd - indexParamStart == 2U )
				{
					venus638lpxData.satellites = (Int8U)(gpsBuffer.buffer[ indexParamStart ] - 0x30) * 10U;
					venus638lpxData.satellites += (Int8U)(gpsBuffer.buffer[ indexParamStart + 1U ] - 0x30);
				}
				indexParam++;
			}
			else if( indexParam == 7U )
			{
				//HDOP
				
				//variable size
				if( indexParamEnd - indexParamStart == 3U )
				{
					venus638lpxData.HDOP = (float)(gpsBuffer.buffer[ indexParamStart ] - 0x30);
					venus638lpxData.HDOP += (float)(gpsBuffer.buffer[ indexParamStart + 2U ] - 0x30) * 0.1F;
				}
				else
				{
					venus638lpxData.HDOP = (float)(gpsBuffer.buffer[ indexParamStart ] - 0x30) * 10U;
					venus638lpxData.HDOP += (float)(gpsBuffer.buffer[ indexParamStart + 1U ] - 0x30);
					venus638lpxData.HDOP += (float)(gpsBuffer.buffer[ indexParamStart + 3U ] - 0x30) * 0.1F;
				}
				indexParam++;
			}
			else if( indexParam == 8U )
			{
				//altitude
				venus638lpxData.altitude = (float)(gpsBuffer.buffer[ indexParamEnd - 1U ] - 0x30) * 0.1F;
				Int8U mult = 1U;
				for( Int8U loop_diz = 0U ; loop_diz < ( ( indexParamEnd - indexParamStart) - 2U ) ; loop_diz++)
				{
					venus638lpxData.altitude += (float)(gpsBuffer.buffer[ indexParamEnd - loop_diz - 3U ] - 0x30) * mult;
					mult *= 10U;
				}
				indexParam++;
			}
			else if( indexParam == 9U )
			{
				//stationId
				
				//fixed size
				if( indexParamEnd - indexParamStart == 4U )
				{
					venus638lpxData.stationId =  (Int16U)(gpsBuffer.buffer[ indexParamStart ] - 0x30) * 1000U;
					venus638lpxData.stationId += (Int16U)(gpsBuffer.buffer[ indexParamStart + 1U ] - 0x30) * 100U;
					venus638lpxData.stationId += (Int16U)(gpsBuffer.buffer[ indexParamStart + 2U ] - 0x30) * 10U;
					venus638lpxData.stationId += (Int16U)(gpsBuffer.buffer[ indexParamStart + 3U ] - 0x30);
					oSuccess = TRUE;
				}
			}
			//after ',' go to next param
			indexParamStart = loop + 1U;
		}
	}
	return oSuccess;
}
static Boolean CmpVENUS638LPXDecodeGPSFrameRMC( void )
{
	Boolean oSuccess = FALSE;
	Int8U indexParam = 0U;
	//start after prefixGP + prefixRMC
	Int8U indexParamStart = 7U;
	Int8U indexParamEnd = 0U;
	//start after prefixGP + prefixRMC
	for(Int8U loop = 7U; loop < (gpsBuffer.index - 4U); loop++)
	{
		if(gpsBuffer.buffer[ loop ] == ',')
		{			
			//record the end
			indexParamEnd = loop;
			if( indexParam == 7U )
			{
				//Course over ground parameter
				
				//fixed size
				if( indexParamEnd - indexParamStart == 5U )
				{
					venus638lpxData.heading = (float)(gpsBuffer.buffer[ indexParamStart ] - 0x30) * 100U;
					venus638lpxData.heading += (float)(gpsBuffer.buffer[ indexParamStart + 1U ] - 0x30) * 10U;
					venus638lpxData.heading += (float)(gpsBuffer.buffer[ indexParamStart + 2U ] - 0x30);
					venus638lpxData.heading+= (float)(gpsBuffer.buffer[ indexParamStart + 4U ] - 0x30) * 0.1F;
					oSuccess = TRUE;
				}
			}
			indexParam++;
			//after ',' go to next param
			indexParamStart = loop + 1U;
		}
	}
	return oSuccess;
}



