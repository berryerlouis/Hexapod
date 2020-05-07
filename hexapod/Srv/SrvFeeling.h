/*
 * SrvFeeling.h
 *
 * Created: 27/07/2017 16:04:46
 *  Author: berryer
 */ 


#ifndef SRVFEELING_H_
#define SRVFEELING_H_

#include "Conf/ConfHard.h"
#include "Tools/tools_typedefs.h"


////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC ENUM///////////////////////////////////////////////
typedef enum
{
	FEELING_CALM,
	FEELING_STRESS
}EFeelingLevel;

typedef enum
{
	FEELING_FEAR,
	FEELING_NEUTRAL,
	FEELING_AGRESSIVE,
}EFeelingState;
////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef struct 
{
	EFeelingLevel feeling;
	EFeelingState state;
	Int32U stressedPeriod;
}SFeeling;

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvFeelingInit ( void ) ;
//Fonction de dispatching d'evenements
void SrvFeelingUpdate (void) ;
void SrvFeelingSetFeelingLevel (EFeelingLevel feeling);
void SrvFeelingSetFeelingStatus (EFeelingState state);
SFeeling *SrvFeelingGetFeeling ( void ) ;

#endif //SRVFEELING_H_