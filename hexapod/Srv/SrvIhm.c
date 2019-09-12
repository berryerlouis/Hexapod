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
	if(DrvButtonPushed(E_BUTTON_SETUP) == TRUE)
	{
		
	}
	else
	{
		
		
	}
}

//platform init done
void SrvIhmPlatformInitDone ( void )
{
	DrvLedSetState(E_LED_1, E_LED_STATE_OFF);
	DrvLedSetState(E_LED_2, E_LED_STATE_OFF);
	DrvLedSetState(E_LED_3, E_LED_STATE_OFF);
}

//platform Arm
void SrvIhmPlatformLedLeft ( ELedState state )
{
	DrvLedSetState(E_LED_2, state);
}
//platform Disarm
void SrvIhmPlatformLedRight ( ELedState state )
{
	DrvLedSetState(E_LED_3, state);
}

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
