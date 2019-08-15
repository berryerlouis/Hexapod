/*
 * SnrUltraSonic.h
 *
 * Created: 06/07/2012 17:29:08
 *  Author: berryer
 */ 


#ifndef CMPULTRASONIC_H
#define CMPULTRASONIC_H

#include "Conf/ConfHard.h"
#include "Drv/DrvTick.h"
#include "Drv/DrvEeprom.h"

////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////


////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef struct
{
	Int16U distance;
} UltraSonicData;


typedef Boolean (*CmpUltraSonicPtrFctInit)		( void );
typedef Boolean (*CmpUltraSonicPtrFctReadData)	( void );

typedef struct
{
	const CmpUltraSonicPtrFctInit init;		
	const CmpUltraSonicPtrFctReadData readData;
}UltraSonicFunctions;

typedef struct
{
	UltraSonicFunctions *functions;
	UltraSonicData *data;
}UltraSonic;

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////


Boolean SnrUltraSonicInit				( void );
Boolean SnrUltraSonicGetDistance     	( void );
UltraSonicData* SnrUltraSonicGetStruct	( void );

#endif //CMPULTRASONIC_H