////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "../Conf/ConfHard.h"

#include "DrvButton.h"
#include "DrvTimer.h"
#include "DrvIo.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define BUTTON_DEBOUNCE_TICK		5U
#define BUTTON_DEBOUNCE_PERIOD		50U
#define BUTTON_DEBOUNCE_THRESHOLD	BUTTON_DEBOUNCE_PERIOD / BUTTON_DEBOUNCE_TICK

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////
//structure configuration initial des leds
typedef struct SSButton
{
	EIoPin pin;
	Boolean enable;
	Int8U debounce_cpt;
	EButtonState state;
	Boolean buttonPushed;
} SButton ;

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
//fct appele par le timer
static void DrvButtonIsrCallbackTimer( void ) ;

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////

//configuration initial des boutons
EIoPin ButtonsPins[ E_NB_BUTTONS ] = BUTTONS_PINS ;
//configuration initial des boutons
volatile SButton MesButtons[ E_NB_BUTTONS ];

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////


//Fonction d'initialisation
//return : TRUE si ok
Boolean DrvButtonInit ( void ) 
{
	Boolean oSuccess = TRUE;
	
	//init de la callback du debounce des boutons
	DrvTimerAddTimer(E_TIMER_BUTTON, BUTTON_DEBOUNCE_TICK, E_TIMER_MODE_PERIODIC, DrvButtonIsrCallbackTimer);

	return oSuccess;
}

//Fonction d'ajout d'un bouton
Boolean DrvButtonAddButton ( Int8U idButton )
{
	Boolean oSuccess = FALSE;
	
	if( idButton < E_NB_BUTTONS )
	{
		MesButtons[ idButton ].pin = ButtonsPins[idButton];
		MesButtons[ idButton ].enable = TRUE;
		MesButtons[ idButton ].debounce_cpt = 0U;
		MesButtons[ idButton ].buttonPushed = FALSE;
		MesButtons[ idButton ].state = E_BUTTON_STATE_RELEASED;
		oSuccess = DrvIoSetPinInput( MesButtons[ idButton ].pin );
	}
	return oSuccess;
}

//Fonction qui retourne l'etat actuel du bouton
EButtonState DrvButtonGetState( Int8U idButton )
{
	return MesButtons[ idButton ].state;
}


//Fonction qui retourne le changement d'etat du bouton
Boolean DrvButtonPushed( Int8U idButton )
{
	Boolean oStatus = FALSE;
	if(MesButtons[ idButton ].state == E_BUTTON_STATE_PUSHED)
	{
		if(	FALSE == MesButtons[ idButton ].buttonPushed )
		{
			MesButtons[ idButton ].buttonPushed = TRUE;
			oStatus = TRUE;
		}
	}
	else
	{
		MesButtons[ idButton ].buttonPushed = FALSE;
		oStatus = FALSE;
	}
	return oStatus;
}

/////////////////////////////////////ISR PRIVATE FUNCTIONS////////////////////////////////////////
//fct appele par le timer
static void DrvButtonIsrCallbackTimer( void )
{
	EIOLevel state ;
	//pour tout les boutons
	for( Int8U i = 0U ; i < E_NB_BUTTONS ; i++)
	{	//si actif
		if( MesButtons[ i ].enable == TRUE )
		{
			//si released
			if(MesButtons[ i ].state == E_BUTTON_STATE_RELEASED)
			{
				state = DrvIoGetPinState(MesButtons[ i ].pin);
				//test du debounce
				if(state == IO_LEVEL_LOW)
				{
					//debounce
					MesButtons[ i ].debounce_cpt++;
					//superieur a 50ms
					if( MesButtons[ i ].debounce_cpt >= BUTTON_DEBOUNCE_THRESHOLD )
					{
						MesButtons[ i ].state = E_BUTTON_STATE_PUSHED;
						//reset du cpt
						MesButtons[ i ].debounce_cpt = 0U;
					}
				}
				else
				{
					//reset du debounce
					MesButtons[ i ].debounce_cpt = 0;
				}
			}
			//si pushed
			else
			{
				state = DrvIoGetPinState(MesButtons[ i ].pin);
				//test du debounce
				if(state == IO_LEVEL_HIGH)
				{
					//debounce
					MesButtons[ i ].debounce_cpt++;
					//superieur a 50ms
					if( MesButtons[ i ].debounce_cpt >= BUTTON_DEBOUNCE_THRESHOLD )
					{
						MesButtons[ i ].state = E_BUTTON_STATE_RELEASED;
						//reset du cpt
						MesButtons[ i ].debounce_cpt = 0U;
					}
				}
				else
				{
					//reset du debounce
					MesButtons[ i ].debounce_cpt = 0;
				}
			}
		}
	}
	
}