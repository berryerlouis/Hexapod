/*
 * DrvAdc.h
 *
 * Created: 14/11/2011 15:18:48
 *  Author: berryer
 */ 
////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#ifndef DRV_BUTTON_H_
#define DRV_BUTTON_H_

#include "../Tools/tools_typedefs.h"

#include "DrvIo.h"

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////

////////////////////////////////////////PUBLIC ENUMS//////////////////////////////////////////////
//etat du bouton
typedef enum
{
	E_BUTTON_STATE_PUSHED,
	E_BUTTON_STATE_RELEASED
}EButtonState;

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean DrvButtonInit ( void ) ;

//Fonction d'ajout de callback a un bouton
Boolean DrvButtonAddButton ( Int8U id_button );

//Fonction qui retourne l'etat actuel du bouton
EButtonState DrvButtonGetState( Int8U id_button );

//Fonction qui retourne le changement d'etat du bouton
Boolean DrvButtonPushed( Int8U id_button );
#endif /* DRV_BUTTON_H_ */
