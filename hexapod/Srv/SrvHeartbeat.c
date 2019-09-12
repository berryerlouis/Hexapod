////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvTick.h"
#include "Drv/DrvLed.h"

#include "SrvHeartbeat.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
uint16_t heartBeatArray[] = {50U, 200U, 25U, 1400U};
uint8_t heartbeatIndex = 0U;
uint32_t prevMillisHeartBeat = 0U;

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////

//Fonction d'initialisation
Boolean SrvHeartbeatInit ( void )
{
	DrvLedInit();
	DrvLedSetPinLed(E_LED_0);
	DrvLedSetToggle(E_LED_0);
	return TRUE;
}
//Fonction de dispatching d'evenements
void SrvHeartbeatUpdate (void)
{
	//heartbeat
	if ((DrvTickGetTimeMs() - prevMillisHeartBeat) > (long)(heartBeatArray[heartbeatIndex]))
	{
		if (++heartbeatIndex > 3U)
		{
			heartbeatIndex = 0U;
		}

		DrvLedSetToggle(E_LED_0);
		prevMillisHeartBeat = DrvTickGetTimeMs();
	}
}

void SrvHeartbeatSetPeriod ( uint8_t period )
{
	//period [0-100]
	heartBeatArray[2U] = 15U + (period / 10U);
	heartBeatArray[1U] = 100U + period;
	heartBeatArray[2U] = 15U + (period / 10U);
	heartBeatArray[3U] = 200U + (10U * period);
}
void SrvHeartbeatStress ( void )
{
	heartBeatArray[2U] = 15U;
	heartBeatArray[1U] = 100U;
	heartBeatArray[2U] = 15U;
	heartBeatArray[3U] = 200U;
}
void SrvHeartbeatCalm ( void )
{
	heartBeatArray[2U] = 25U;
	heartBeatArray[1U] = 200U;
	heartBeatArray[2U] = 25U;
	heartBeatArray[3U] = 1400U;
}