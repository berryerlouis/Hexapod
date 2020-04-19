////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvTick.h"
#include "Drv/DrvLed.h"
#include "Drv/DrvButton.h"
#include "SrvIhm.h"
#include "SrvComm.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
Int32U timerLed[E_NB_LEDS];
Boolean timerEnable[E_NB_LEDS];
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
	DrvLedSetPinLed(E_LED_1);
	DrvLedSetPinLed(E_LED_2);
	DrvLedSetPinLed(E_LED_3);
	
	//init done
	SrvIhmPlatformInitDone();
    return oSuccess;
}

//Fonction de dispatching d'evenements
void SrvIhmUpdate ( void )
{
	//test the button status
	//if(DrvButtonPushed(E_BUTTON_SETUP) == TRUE)

	
	for(ELed led = E_LED_0 ; led < E_NB_LEDS ; led ++)
	{
		if((timerEnable[led] == TRUE) && (timerLed[led] < DrvTickGetTimeMs()))
		{
			DrvLedSetState(led, E_LED_STATE_OFF);
			timerEnable[led] = FALSE;
		}
	}
}

//platform init done
void SrvIhmPlatformInitDone ( void )
{
	DrvLedSetState(E_LED_1, E_LED_STATE_OFF);
	DrvLedSetState(E_LED_2, E_LED_STATE_OFF);
	DrvLedSetState(E_LED_3, E_LED_STATE_OFF);
}

//platform Set Left LED Time On
void SrvIhmPlatformLeftLedTimeOn ( ELedState state, Int16U delayMs )
{
	timerLed[E_LED_2] = delayMs + DrvTickGetTimeMs();
	timerEnable[E_LED_2] = TRUE;
	DrvLedSetState(E_LED_2, state);
}
//platform Set Right LED Time On
void SrvIhmPlatformRightLedTimeOn ( ELedState state, Int16U delayMs )
{
	timerLed[E_LED_3] = delayMs + DrvTickGetTimeMs();
	timerEnable[E_LED_3] = TRUE;
	DrvLedSetState(E_LED_3, state);
}

//platform Set Left LED
void SrvIhmPlatformLeftLed ( ELedState state )
{
	DrvLedSetState(E_LED_2, state);
}
//platform Set Right LED
void SrvIhmPlatformRightLed ( ELedState state )
{
	DrvLedSetState(E_LED_3, state);
}

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
