/*
 * SrvCore.h
 *
 * Created: 27/07/2017 16:04:46
 *  Author: berryer
 */ 


#ifndef SRVCORE_H_
#define SRVCORE_H_

#include "Conf/ConfHard.h"
#include "Tools/tools_typedefs.h"


////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC ENUM///////////////////////////////////////////////

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////

typedef Boolean (*ptrfctDriverInit)(void);
typedef struct
{
	Int8U index;
	ptrfctDriverInit init;
	Int32U updateTime;
}SCoreDriver;

typedef Boolean (*ptrfctServiceInit)(void);
typedef void (*ptrfctServiceUpdate)(void);
typedef struct
{
	Int8U index;
	ptrfctServiceInit init;
	ptrfctServiceUpdate update;
	Int32U updateTime;
	Int32U updateTimeMax;
}SCoreService;

typedef struct
{
	SCoreDriver *drivers;
	Int8U nbDrivers;
	SCoreService *services;
	Int8U nbServices;
	Int32U updateTime;
	Int32U updateTimeMax;
}SCore;


////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvCoreInit ( void ) ;

Boolean SrvCoreLoop ( void ) ;

SCoreService *SrvCoreGetServices ( void ) ;

#endif //SRVCORE_H_