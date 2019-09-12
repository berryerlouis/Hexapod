////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "SrvCore.h"


#include "Drv/DrvTick.h"
#include "Drv/DrvEeprom.h"
#include "Drv/DrvTimer.h"
#include "Drv/DrvInterrupt.h"
#include "Drv/DrvLed.h"
#include "Drv/DrvUart.h"
#include "Drv/DrvTwi.h"


#include "SrvHeartbeat.h"
#include "SrvIhm.h"
#include "SrvWalk.h"
#include "SrvComm.h"
#include "SrvBody.h"
#include "SrvDisplay.h"
#include "SrvBattery.h"
#include "SrvUltrason.h"
#include "SrvFeeling.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
//permits to compute the main loop time
Int32U lastLoopTime = 0U;
////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////


SServices coreServices [] =
{
	{
		SrvCommInit,
		SrvCommUpdate,
		0UL,
		0UL
	},
	{
		SrvBatteryInit,
		SrvBatteryUpdate,
		0UL,
		0UL
	},
	{
		SrvIhmInit,
		SrvIhmUpdate,
		0UL,
		0UL
	},
	{
		SrvBodyInit,
		SrvBodyUpdate,
		0UL,
		0UL
	},
	{
		SrvWalkInit,
		SrvWalkUpdate,
		0UL,
		0UL
	},
	{
		SrvUltrasonInit,
		SrvUltrasonUpdate,
		0UL,
		0UL
	},
	{
		SrvHeartbeatInit,
		SrvHeartbeatUpdate,
		0UL,
		0UL
	},
	{
		SrvFeelingInit,
		SrvFeelingUpdate,
		0UL,
		0UL
	},
	{
		SrvDisplayInit,
		SrvDisplayUpdate,
		0UL,
		0UL
	}
};

#define NB_SERVICES (Int8U)((Int8U)sizeof(coreServices)/sizeof(SServices))
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////



//Fonction d'initialisation
Boolean SrvCoreInit ( void ) 
{
	Boolean oSuccess = TRUE;
	
	//Init Drivers
	oSuccess == TRUE ? oSuccess = DrvTickInit() : FALSE;
	oSuccess == TRUE ? oSuccess = DrvTimerInit() : FALSE;
	oSuccess == TRUE ? oSuccess = DrvEepromInit() : FALSE;
	oSuccess == TRUE ? oSuccess = DrvUartInit( E_UART_0, UART_SPEED_115200 ) : FALSE;
	oSuccess == TRUE ? oSuccess = DrvTwiInit( TWI_SPEED_400K ) : FALSE;

	
	for(uint8_t i = 0U ; i < NB_SERVICES ; i++)
	{
		oSuccess == TRUE ? oSuccess = coreServices[i].init() : FALSE;
		coreServices[i].updateTime = 0UL;
	}
		
	//Reset time
	DrvTimerTickReset();
	
	return oSuccess;
}

Boolean SrvCoreLoop ( void ) 
{
	Boolean oSuccess = FALSE;
	for(uint8_t i = 0U ; i < NB_SERVICES ; i++)
	{
		//get time
		Int32U now = DrvTickGetTimeUs();
		coreServices[i].update();
		//get loop update time
		coreServices[i].updateTime = DrvTickGetTimeUs() - now;
		//get the max time
		if(coreServices[i].updateTime > coreServices[i].updateTimeMax)
		{
			coreServices[i].updateTimeMax = coreServices[i].updateTime;
		}
	}
	return oSuccess;
}


SServices *SrvCoreGetServices ( void )
{
	return coreServices;
}