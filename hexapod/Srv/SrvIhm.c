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

typedef struct
{
	Boolean enable;
	Int32U updateTime;
}SIhmLed;

SIhmLed leds[E_NB_LEDS];
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
	//DrvButtonAddButton(E_BUTTON_SETUP);
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
	for(ELed led = E_LED_0 ; led < E_NB_LEDS ; led ++)
	{
		if((leds[led].enable == TRUE) && (leds[led].updateTime < DrvTickGetTimeMs()))
		{
			DrvLedSetState(led, E_LED_STATE_OFF);
			leds[led].enable = FALSE;
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
	leds[E_LED_2].updateTime = delayMs + DrvTickGetTimeMs();
	leds[E_LED_2].enable = TRUE;
	DrvLedSetState(E_LED_2, state);
}
//platform Set Right LED Time On
void SrvIhmPlatformRightLedTimeOn ( ELedState state, Int16U delayMs )
{
	leds[E_LED_3].updateTime = delayMs + DrvTickGetTimeMs();
	leds[E_LED_3].enable = TRUE;
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
