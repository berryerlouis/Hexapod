////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "../Conf/ConfHard.h"

#include "../Drv/DrvTick.h"
#include "../Drv/DrvLed.h"
#include "../Drv/DrvButton.h"
#include "SrvIhm.h"
#include "SrvComm.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
uint16_t heartBeatArray[] = {50U, 100U, 15U, 1200U};
uint8_t heartbeatIndex = 0U;
uint32_t prevMillisHeartBeat = 0U;

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
//return : TRUE si ok
Boolean SrvIhmInit ( void )
{
    Boolean oSuccess = TRUE;

	//init buttons & leds
	DrvButtonInit();
	DrvLedInit();

	//create button
	DrvButtonAddButton(E_BUTTON_SETUP);
	//create the LEDs
	DrvLedSetPinLed(E_LED_0);
	DrvLedSetPinLed(E_LED_1);
	DrvLedSetPinLed(E_LED_2);
	DrvLedSetPinLed(E_LED_3);
	
    return oSuccess;
}

//Fonction de dispatching d'evenements
void SrvIhmDispatcher ( void )
{
	//start hearbeating
	SrvIhmHeartbeat();
	//test the button status
	if(DrvButtonPushed(E_BUTTON_SETUP) == TRUE)
	{
		
	}
	else
	{
		
		
	}
}


//heartbeat
void SrvIhmHeartbeat ( void )
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

//platform init start
void SrvIhmPlatformInitStart ( void )
{
	DrvLedSetState(E_LED_0, E_LED_STATE_ON);
	DrvLedSetState(E_LED_1, E_LED_STATE_ON);
	DrvLedSetState(E_LED_2, E_LED_STATE_ON);
	DrvLedSetState(E_LED_3, E_LED_STATE_ON);
}

//platform init done
void SrvIhmPlatformInitDone ( void )
{
	DrvLedSetState(E_LED_0, E_LED_STATE_ON);
	DrvLedSetState(E_LED_1, E_LED_STATE_OFF);
	DrvLedSetState(E_LED_2, E_LED_STATE_OFF);
	DrvLedSetState(E_LED_3, E_LED_STATE_OFF);
}

//platform Arm
void SrvIhmPlatformArm ( void )
{
	DrvLedSetBlinkMode(E_LED_0, 125U, 125U);
	DrvLedSetState(E_LED_1, E_LED_STATE_OFF);
}
//platform Disarm
void SrvIhmPlatformDisarm ( void )
{
	DrvLedSetState(E_LED_1, E_LED_STATE_OFF);
	DrvLedSetState(E_LED_0, E_LED_STATE_ON);
}
//platform Error
void SrvIhmPlatformError ( void ) 
{
	DrvLedSetBlinkMode(E_LED_0, 125U, 125U);
	DrvLedSetBlinkMode(E_LED_1, 125U, 125U);
	DrvLedSetBlinkMode(E_LED_2, 125U, 125U);
	DrvLedSetBlinkMode(E_LED_3, 125U, 125U);
}
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
