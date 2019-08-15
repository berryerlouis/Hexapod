////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include   "SnrBarometer.h"

#ifdef BAR_BMP085
	#include   "Cmps/CmpBMP085.h"
#endif // BAR_BMP085 
////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
static Barometer barometer;

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
Boolean SnrBarometerInit ( float altitude )
{	
#ifdef BAR_BMP085
	barometer.data = &bmp085Data;
	barometer.functions = &bmp085;
#endif // BAR_BMP085

	//init du composant
	return barometer.functions->init(altitude);
}

Boolean SnrBarometerUpdate( void )
{
	return barometer.functions->update();
}
float SnrBarometerGetTemperature( void )
{
	return barometer.data->temperature;
}

Int16U SnrBarometerGetSeaLevelPressure ( void )
{
	return barometer.data->temperature;
}

Int32S SnrBarometerGetPressure( void )
{
	return barometer.data->pressure;
}
Int16U SnrBarometerUpdateAltitude( void )
{
	return barometer.data->altitude;
}
E_Weather SnrBarometerUpdateWeather( void )
{
	return barometer.data->weather;
}

BarometerData* SnrBarometerGetStruct( void )
{
	return barometer.data;
}


////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
