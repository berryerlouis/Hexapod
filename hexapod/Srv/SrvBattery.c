////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvAdc.h"
#include "Srv/SrvBattery.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
EBatteryLevel level;
uint16_t batteryPercentage;
////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////



//Fonction d'initialisation
Boolean SrvBatteryInit ( void ) 
{
	level = BATTERY_0;
	batteryPercentage = 0;
	return DrvAdcInit();
}

//Fonction de dispatching d'evenements
void SrvBatteryUpdate (void)
{
	#define MAX_VOLATGE 152U
	#define MIN_VOLATGE 130U
	batteryPercentage = (uint16_t)DrvAdcRead(EIO_PIN_A_0);
	
	batteryPercentage = (batteryPercentage * 100U) / MAX_VOLATGE ;
	//return (x - 0) * (out_max - out_min) / (in_max - in_min) + out_min;
	
	if(batteryPercentage >= 95)	level = BATTERY_100;
	else if(batteryPercentage >= 75)	level = BATTERY_75;
	else if(batteryPercentage >= 50)	level = BATTERY_50;
	else if(batteryPercentage >= 25)	level = BATTERY_25;
	else level = BATTERY_0;
}


EBatteryLevel SrvBatteryGetLevel (void)
{
	return level;
}
uint8_t SrvBatteryGetValue (void) 
{
	return batteryPercentage;
}