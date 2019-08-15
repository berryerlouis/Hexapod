////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "../Conf/ConfHard.h"

#include "DrvLed.h"
#include "DrvTimer.h"
#include "DrvIo.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define LED_PERIOD	1U
////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////
//structure configuration initial des leds
typedef struct SSLed
{
	EIoPin pin;
	ELedState state;
	ELedMode mode;
	Int16U cptOnStop;
	Int16U cptOn;
	Int16U cptOff;
	Int16U cptOffStop;
} SLed ;

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
//fct appele par le timer
static void DrvLedIsrCallbackTimer( void ) ;

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
//configuration initial des boutons
EIoPin LedsPins[ E_NB_LEDS ] = LEDS_PINS ;
//configuration initial des leds
volatile SLed MesLeds[ E_NB_LEDS ];

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////


//Fonction d'initialisation
//return : TRUE si ok
Boolean DrvLedInit ( void )
{
	Boolean oSuccess = TRUE;
	
	//init de la callback du debounce des leds
	DrvTimerAddTimer(E_TIMER_LED, LED_PERIOD, E_TIMER_MODE_PERIODIC, DrvLedIsrCallbackTimer);

	return oSuccess;
}


//Fonction d'ajout de led
Boolean DrvLedSetPinLed ( Int8U idLed )
{
	Boolean oSuccess = FALSE;
	
	if( idLed < E_NB_LEDS )
	{
		MesLeds[ idLed ].pin = LedsPins[ idLed ];
		MesLeds[ idLed ].state = E_LED_STATE_OFF;
		MesLeds[ idLed ].mode = E_LED_ON_OFF;
		MesLeds[ idLed ].cptOnStop = 0U;
		MesLeds[ idLed ].cptOn = 0U;
		MesLeds[ idLed ].cptOff = 0U;
		MesLeds[ idLed ].cptOffStop = 0U;
		oSuccess = DrvIoSetPinOutput( MesLeds[ idLed ].pin );
	}
	return oSuccess;
}

//Fonction de modification d'etat de la led en ON
Boolean DrvLedSetOn ( Int8U idLed )
{
	Boolean oSuccess = FALSE;
	
	if( idLed < E_NB_LEDS )
	{
		MesLeds[ idLed ].mode = E_LED_ON_OFF;
		MesLeds[ idLed ].state = E_LED_STATE_ON;
		DrvIoSetPinState(MesLeds[ idLed ].pin,TRUE);
	}
	return oSuccess;
}

//Fonction de modification d'etat de la led en OFF
Boolean DrvLedSetOff ( Int8U idLed )
{
	Boolean oSuccess = FALSE;
	
	if( idLed < E_NB_LEDS )
	{
		MesLeds[ idLed ].mode = E_LED_ON_OFF;
		MesLeds[ idLed ].state = E_LED_STATE_OFF;
		DrvIoSetPinState(MesLeds[ idLed ].pin,FALSE);
	}
	return oSuccess;
}

//Fonction de modification d'etat de la led en TOGGLE
Boolean DrvLedSetToggle ( Int8U idLed )
{
	Boolean oSuccess = FALSE;
	
	if( idLed < E_NB_LEDS )
	{
		MesLeds[ idLed ].mode = E_LED_ON_OFF;
		if( MesLeds[ idLed ].state == E_LED_STATE_ON )
		{
			MesLeds[ idLed ].state = E_LED_STATE_OFF;
			DrvIoSetPinState(MesLeds[ idLed ].pin,TRUE);
		}
		else if( MesLeds[ idLed ].state == E_LED_STATE_OFF )
		{
			MesLeds[ idLed ].state = E_LED_STATE_ON;
			DrvIoSetPinState(MesLeds[ idLed ].pin,FALSE);
		}
	}
	return oSuccess;
}

//Fonction de modification d'etat de la led
Boolean DrvLedSetState ( Int8U idLed, ELedState state )
{
	Boolean oSuccess = FALSE;
	
	if( idLed < E_NB_LEDS )
	{
		MesLeds[ idLed ].mode = E_LED_ON_OFF;
		if( state == E_LED_STATE_ON )
		{
			MesLeds[ idLed ].state = E_LED_STATE_ON;
			DrvIoSetPinState(MesLeds[ idLed ].pin,FALSE);
		}
		else if( state == E_LED_STATE_OFF )
		{
			MesLeds[ idLed ].state = E_LED_STATE_OFF;
			DrvIoSetPinState(MesLeds[ idLed ].pin,TRUE);
		}
	}
	return oSuccess;
}

//Fonction de recuperation d'etat de la led
ELedState DrvLedGetState ( Int8U idLed )
{
	if( idLed < E_NB_LEDS )
	{
		return MesLeds[ idLed ].state;
	}
	return E_LED_STATE_NONE;
}

//Fonction de modification du mode de la led
Boolean DrvLedSetMode ( Int8U idLed, ELedMode mode )
{
	Boolean oSuccess = FALSE;
	
	if( idLed < E_NB_LEDS )
	{
		MesLeds[ idLed ].mode = mode;
	}
	return oSuccess;
}


//Fonction de modification du mode de la led
Boolean DrvLedSetBlinkMode ( Int8U idLed, Int16U delay_on , Int16U delay_off )
{
	Boolean oSuccess = FALSE;
	
	if( idLed < E_NB_LEDS )
	{
		MesLeds[ idLed ].mode = E_LED_BLINK;
		MesLeds[ idLed ].cptOnStop = delay_on;
		MesLeds[ idLed ].cptOffStop = delay_off;
	}
	return oSuccess;
}

/////////////////////////////////////ISR PRIVATE FUNCTIONS////////////////////////////////////////
//fct appele par le timer
static void DrvLedIsrCallbackTimer( void )
{
	//pour tout les boutons
	for( Int8U idLed = 0U ; idLed < E_NB_LEDS ; idLed++)
	{	
		//mode est blink
		if( MesLeds[ idLed ].mode == E_LED_BLINK )
		{
			//si on est ON
			if( MesLeds[ idLed ].state == E_LED_STATE_ON )
			{
				if( MesLeds[ idLed ].cptOn++ > MesLeds[ idLed ].cptOnStop )
				{
					MesLeds[ idLed ].state = E_LED_STATE_OFF;
					DrvIoSetPinState(MesLeds[ idLed ].pin,FALSE);
					MesLeds[ idLed ].cptOn = 0;
				}
			}
			//si on est OFF
			if( MesLeds[ idLed ].state == E_LED_STATE_OFF )
			{
				if( MesLeds[ idLed ].cptOff++ > MesLeds[ idLed ].cptOffStop )
				{
					MesLeds[ idLed ].state = E_LED_STATE_ON;
					DrvIoSetPinState(MesLeds[ idLed ].pin,TRUE);
					MesLeds[ idLed ].cptOff = 0;
				}
			}
		}
	}
}