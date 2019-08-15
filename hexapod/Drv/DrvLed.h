/*
 * DrvAdc.h
 *
 * Created: 14/11/2011 15:18:48
 *  Author: berryer
 */ 
////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#ifndef DRV_LED_H_
#define DRV_LED_H_

#include "../Tools/tools_typedefs.h"

#include "DrvIo.h"

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////

////////////////////////////////////////PUBLIC ENUMS//////////////////////////////////////////////
//etat de la led
typedef enum
{
	E_LED_STATE_ON ,
	E_LED_STATE_OFF,
	E_LED_STATE_NONE,
}ELedState;

//mode de la led
typedef enum
{
	E_LED_ON_OFF,
	E_LED_BLINK,
}ELedMode;

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////


//////////////////////////////////////////PUBLIC PTR FCTS//////////////////////////////////////////

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean DrvLedInit ( void ) ;

//Fonction d'ajout de led
Boolean DrvLedSetPinLed ( Int8U id_led );

//Fonction de modification d'etat de la led en ON
Boolean DrvLedSetOn ( Int8U id_led );

//Fonction de modification d'etat de la led en OFF
Boolean DrvLedSetOff ( Int8U id_led );

//Fonction de modification d'etat de la led en TOGGLE
Boolean DrvLedSetToggle ( Int8U id_led );

//Fonction de modification d'etat de la led
Boolean DrvLedSetState ( Int8U id_led, ELedState state );

//Fonction de modification du mode de la led
Boolean DrvLedSetMode ( Int8U id_led, ELedMode mode );

//Fonction de recuperation d'etat de la led
ELedState DrvLedGetState ( Int8U id_led );

//Fonction de modification du mode de la led
Boolean DrvLedSetBlinkMode ( Int8U id_led, Int16U delay_on , Int16U delay_off );
#endif /* DRV_LED_H_ */
