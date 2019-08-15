/*
 * BMP085.c
 *
 * Created: 03/07/2012 13:48:49
 *  Author: berryer
 */
/////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
#include "CmpBMP085.h"
 
#include "Drv/DrvTwi.h"
#include "Drv/DrvTick.h"
#include "Drv/DrvEeprom.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define SEA_PRESSURE	101325.0F
/////////////////////////////////////////PRIVATE STRUCTIURES///////////////////////////////////////
Int16S ac1;
Int16S ac2;
Int16S ac3;
Int16U ac4;
Int16U ac5;
Int16U ac6;
Int16S b1;
Int16S b2;
Int16S mb;
Int16S mc;
Int16S md;
Int32S b5;  

// BMP085 Modes
typedef enum
{
	MODE_ULTRA_LOW_POWER,//oversampling=0, internalsamples=1, maxconvtimepressure=4.5ms, avgcurrent=3uA, RMSnoise_hPA=0.06, RMSnoise_m=0.5
	MODE_STANDARD		,//oversampling=1, internalsamples=2, maxconvtimepressure=7.5ms, avgcurrent=5uA, RMSnoise_hPA=0.05, RMSnoise_m=0.4
	MODE_HIGHRES		,//oversampling=2, internalsamples=4, maxconvtimepressure=13.5ms, avgcurrent=7uA, RMSnoise_hPA=0.04, RMSnoise_m=0.3
	MODE_ULTRA_HIGHRES	 //oversampling=3, internalsamples=8, maxconvtimepressure=25.5ms, avgcurrent=12uA, RMSnoise_hPA=0.03, RMSnoise_m=0.25
}E_Bmp085Mode;


////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
static Boolean CmpBMP085Init( float altitude );
static Boolean CmpBMP085Update( void ) ;

static void CmpBMP085ReadCalibration( void );
static void CmpBMP085ReadUtUp( void ) ;
static Int16U CmpBMP085UpdateAltitude( void );
static Int16U CmpBMP085GetSeaLevelPressure( void );
static E_Weather CmpBMP085UpdateWeather( void );

/////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
Int8U stepBmp085 = 0;

E_Bmp085Mode bmp085Mode; 

//definition de la structure des fonctions de accelerometer pour le composant HMC5886
BarometerFunctions bmp085 =
{
	CmpBMP085Init,
	CmpBMP085Update
};

//definition de la structure des data accelerometer pour le composant HMC5886
BarometerData bmp085Data;

//altitude knon by user
float bmp085Altitude = 0.0F;

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

//Initialisation du barometre
static Boolean CmpBMP085Init( float altitude )
{
	Boolean o_success = FALSE;
	
	bmp085Mode=MODE_ULTRA_HIGHRES;
	CmpBMP085ReadCalibration();
	CmpBMP085Update();
	bmp085Data.weather = WEATHER_NONE;
	bmp085Altitude = altitude;
	o_success = TRUE;
	return o_success;
}


//read the two parameters UT and UP
static Boolean CmpBMP085Update( void )
{
	CmpBMP085ReadUtUp( ) ;
	return TRUE;
}

//Get Sea Level Pressure
static Int16U CmpBMP085GetSeaLevelPressure( void )
{
	return bmp085Data.seaLevelPressure =  (Int32U)(bmp085Data.pressure / pow(1.0 - bmp085Altitude/44330.0F, 5.255F)); 
}

//Get Altitude
static Int16U CmpBMP085UpdateAltitude( void )
{
	return bmp085Data.altitude = (float)44330.0F * (1.0F - pow(((float) bmp085Data.pressure/bmp085Data.seaLevelPressure), 0.190295F));
}

//Get weather 
static E_Weather CmpBMP085UpdateWeather( void )
{	
	// calcul de la presion attendu a notre altitudeconst
	float currentAltitude = (float)bmp085Data.altitude; // current altitude in METERS
	static float ePressure = 0;
	ePressure = SEA_PRESSURE * pow((1-currentAltitude/44330.0F), 5.255F);
	// on compare par rapport a la pression mesurée
	Int16S weatherDiff = (Int16S)( bmp085Data.pressure - ePressure );
	//on retourne le type de temps
	if(weatherDiff > 250)
	{
		bmp085Data.weather = WEATHER_SUNNY;
	}
	else if ((weatherDiff <= 250) && (weatherDiff >= -250))
	{
		bmp085Data.weather = WEATHER_CLOUDY;
	}
	else
	{
		bmp085Data.weather = WEATHER_RAIN;
	}
	return bmp085Data.weather;
}


//read all value
static void CmpBMP085ReadCalibration( void )
{
  //read calibration
  Int8U datum = 0U;
  
  DrvTwiReadReg( BMP085_ADDRESS, CAL_AC1, &datum );
  ac1 = datum << 8;
  DrvTwiReadReg( BMP085_ADDRESS, CAL_AC1 + 1, &datum );
  ac1 |= datum;
  
  DrvTwiReadReg( BMP085_ADDRESS, CAL_AC2, &datum );
  ac2 = datum << 8;
  DrvTwiReadReg( BMP085_ADDRESS, CAL_AC2 + 1, &datum );
  ac2 |= datum;
  
  DrvTwiReadReg( BMP085_ADDRESS, CAL_AC3, &datum );
  ac3 = datum << 8;
  DrvTwiReadReg( BMP085_ADDRESS, CAL_AC3 + 1, &datum );
  ac3 |= datum;
  
  DrvTwiReadReg( BMP085_ADDRESS, CAL_AC4, &datum );
  ac4 = datum << 8;
  DrvTwiReadReg( BMP085_ADDRESS, CAL_AC4 + 1, &datum );
  ac4 |= datum;
  
  DrvTwiReadReg( BMP085_ADDRESS, CAL_AC5, &datum );
  ac5 = datum << 8;
  DrvTwiReadReg( BMP085_ADDRESS, CAL_AC5 + 1, &datum );
  ac5 |= datum;
  
  DrvTwiReadReg( BMP085_ADDRESS, CAL_AC6, &datum );
  ac6 = datum << 8;
  DrvTwiReadReg( BMP085_ADDRESS, CAL_AC6 + 1, &datum );
  ac6|= datum;
  
  DrvTwiReadReg( BMP085_ADDRESS, CAL_B1, &datum );
  b1 = datum << 8;
  DrvTwiReadReg( BMP085_ADDRESS, CAL_B1 + 1, &datum );
  b1|= datum;
  
  DrvTwiReadReg( BMP085_ADDRESS, CAL_B2, &datum );
  b2 = datum << 8;
  DrvTwiReadReg( BMP085_ADDRESS, CAL_B2 + 1, &datum );
  b2|= datum;
  
  b5 = 0;
  
  DrvTwiReadReg( BMP085_ADDRESS, CAL_MB, &datum );
  mb = datum << 8;
  DrvTwiReadReg( BMP085_ADDRESS, CAL_MB + 1, &datum );
  mb|= datum;
  
  DrvTwiReadReg( BMP085_ADDRESS, CAL_MC, &datum );
  mc = datum << 8;
  DrvTwiReadReg( BMP085_ADDRESS, CAL_MC + 1, &datum );
  mc|= datum;
  
  DrvTwiReadReg( BMP085_ADDRESS, CAL_MD, &datum );
  md = datum << 8;
  DrvTwiReadReg( BMP085_ADDRESS, CAL_MD + 1, &datum );
  md|= datum;
}

//read the two parameters UT and UP
static void CmpBMP085ReadUtUp( void ) 
{
	Int32S x1, x2, x3, b3, b6, p ;
	Int32U b4, b7;
	Int8U data[3];
	
	switch ( stepBmp085 )
	{
		case 0:
			//temperature reading
			DrvTwiWriteReg( BMP085_ADDRESS, CONTROL, READ_TEMPERATURE );
			stepBmp085++;
		break;
		
		case 1:		
		
			// Read two uint8_ts from registers 0xF6 and 0xF7
			DrvTwiReadRegBuf( BMP085_ADDRESS, CONTROL_OUTPUT, data, 2U );
			bmp085Data.rawData.ut = (Int16U)((((Int16U)data[ 0U ]) << 8) | data[ 1U ] );
		
			x1 = (((Int32S)bmp085Data.rawData.ut - (Int32S)ac6)*(Int32S)ac5) >> 15;
			x2 = ((Int32S)mc << 11)/(x1 + md);
			b5 = x1 + x2;
			
			float temp = ((b5 + 8)>>4);
			bmp085Data.temperature = (Int16S)(temp / 10 - 1.5);
			
			//pressure reading
			DrvTwiWriteReg( BMP085_ADDRESS, CONTROL, READ_PRESSURE + (bmp085Mode<<6) );
			stepBmp085++;
		break;
		
		case 2:
		
			// Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
			DrvTwiReadRegBuf( BMP085_ADDRESS, CONTROL_OUTPUT, data, 3U );		
			bmp085Data.rawData.up = (((Int32U) data[ 0U ] << 16) | ((Int32U) data[ 1U ] << 8) | (Int32U) data[ 2U ]) >> (8 - bmp085Mode);
		
			b6 = b5 - 4000;
			// Calculate B3
			x1 = (b2 * (b6 * b6)>>12)>>11;
			x2 = (ac2 * b6)>>11;
			x3 = x1 + x2;
			b3 = (((((Int32S)ac1)*4 + x3)<<bmp085Mode) + 2)>>2;
		
			// Calculate B4
			x1 = (ac3 * b6)>>13;
			x2 = (b1 * ((b6 * b6)>>12))>>16;
			x3 = ((x1 + x2) + 2)>>2;
			b4 = (ac4 * (Int32U)(x3 + 32768))>>15;
		
			b7 = ((Int32U)(bmp085Data.rawData.up - b3) * (50000>>bmp085Mode));
			if (b7 < 0x80000000)
			p = (b7<<1)/b4;
			else
			p = (b7/b4)<<1;
		
			x1 = (p>>8) * (p>>8);
			x1 = (x1 * 3038)>>16;
			x2 = (-7357 * p)>>16;
			p += (x1 + x2 + 3791)>>4;
			
			bmp085Data.pressure = p;
			stepBmp085++;
		break;
		
		case 3:
			CmpBMP085GetSeaLevelPressure();
			stepBmp085++;
		break;
		
		case 4:
			CmpBMP085UpdateAltitude();
			stepBmp085++;
		break;
		case 5:
			CmpBMP085UpdateWeather();
			//on reboucle
			stepBmp085 = 0U;
		break;
		
		
		default: 
			//on reinit
			stepBmp085 = 0U;
		break;
		
	}
	
	
	
}

