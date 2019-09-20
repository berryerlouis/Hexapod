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
SCoreDriver coreDrivers [] =
{
	{
		0U,
		DrvTickInit,
		0UL,
	},
	{
		1U,
		DrvTimerInit,
		0UL,
	},
	{
		2U,
		DrvEepromInit,
		0UL,
	},
	{
		3U,
		DrvUartInit,
		0UL,
	},
	{
		4U,
		DrvTwiInit,
		0UL,
	},
};

#define NB_CORE_DRIVERS (Int8U)((Int8U)sizeof(coreDrivers)/sizeof(SCoreDriver))

SCoreService coreServices [] =
{
	{
		0U,
		SrvCommInit,
		SrvCommUpdate,
		0UL,
		0UL
	},
	{
		1U,
		SrvBatteryInit,
		SrvBatteryUpdate,
		0UL,
		0UL
	},
	{
		2U,
		SrvIhmInit,
		SrvIhmUpdate,
		0UL,
		0UL
	},
	{
		3U,
		SrvBodyInit,
		SrvBodyUpdate,
		0UL,
		0UL
	},
	{
		4U,
		SrvWalkInit,
		SrvWalkUpdate,
		0UL,
		0UL
	},
	{
		5U,
		SrvUltrasonInit,
		SrvUltrasonUpdate,
		0UL,
		0UL
	},
	{
		6U,
		SrvHeartbeatInit,
		SrvHeartbeatUpdate,
		0UL,
		0UL
	},
	{
		7U,
		SrvFeelingInit,
		SrvFeelingUpdate,
		0UL,
		0UL
	},
	{
		8U,
		SrvDisplayInit,
		SrvDisplayUpdate,
		0UL,
		0UL
	}
};

#define NB_CORE_SERVICES (Int8U)((Int8U)sizeof(coreServices)/sizeof(SCoreService))

SCore core = 
{
	.drivers = coreDrivers,
	.nbDrivers = NB_CORE_DRIVERS,
	.services = coreServices,
	.nbServices = NB_CORE_SERVICES,
	.updateTime = 0U,
	.updateTimeMax = 0U,
};
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////



//Fonction d'initialisation
Boolean SrvCoreInit ( void ) 
{
	Boolean oSuccess = TRUE;
	DrvTwiInit();
	//Init Core Drivers
	for(uint8_t i = 0U ; i < core.nbDrivers ; i++)
	{
		Int32U nowDrvCore = DrvTickGetTimeUs();
		oSuccess == TRUE ? oSuccess = core.drivers[i].init() : FALSE;
		//get loop update time
		core.drivers[i].updateTime = DrvTickGetTimeUs() - nowDrvCore;
		core.drivers[i].index = i;
	}
	
	//Init Core Services
	for(uint8_t i = 0U ; i < NB_CORE_SERVICES ; i++)
	{
		Int32U nowSrvCore = DrvTickGetTimeUs();
		oSuccess == TRUE ? oSuccess = coreServices[i].init() : FALSE;
		core.services[i].index = i;
		core.services[i].updateTime = DrvTickGetTimeUs() - nowSrvCore;
	}
		
	//Reset time
	DrvTimerTickReset();
	
	return oSuccess;
}

Boolean SrvCoreLoop ( void ) 
{
	Boolean oSuccess = TRUE;
	//get time
	Int32U nowSrvCore = DrvTickGetTimeUs();
	for(uint8_t i = 0U ; i < NB_CORE_SERVICES ; i++)
	{
		//get time
		Int32U now = DrvTickGetTimeUs();
		core.services[i].update();
		//get loop update time
		core.services[i].updateTime = DrvTickGetTimeUs() - now;
		//get the max time
		core.services[i].updateTimeMax = MAX(core.services[i].updateTime,core.services[i].updateTimeMax);
	}
	//get loop update time
	core.updateTime = DrvTickGetTimeUs() - nowSrvCore;
	//get the max time
	core.updateTimeMax = MAX(core.updateTime,core.updateTimeMax);
	
	return oSuccess;
}


SCoreService *SrvCoreGetServices ( void )
{
	return coreServices;
}