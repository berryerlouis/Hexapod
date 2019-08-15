////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include   "SnrVariometer.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
static VariometerData data;
static Variometer variometer;
float intervalVario = 0.0F;
Int32U lastreadVario = 0U;

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
Boolean SnrVariometerInit ( void )
{	
	//init du composant
	variometer.data = &data;
	return TRUE;
}

Int16S SnrVariometerGetVariation ( float altitude )
{
	// ********************* Calcul du temps de cycle *************************
	Int32U nowVario = DrvTickGetTimeUs();
	intervalVario = (float)(nowVario - lastreadVario) / 1000000.0F;
	lastreadVario = nowVario;
			
	variometer.data->variation = variometer.data->variation * 0.985 + ((altitude - variometer.data->lastAltitude) / intervalVario);
	variometer.data->lastAltitude = altitude;
	return variometer.data->variation;
}

VariometerData* SnrVariometerGetStruct( void )
{
	return variometer.data;
}


////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
