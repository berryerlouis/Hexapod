/*
 * DrvBootload.h
 *
 * Created: 06/07/2012 16:35:15
 *  Author: berryer
 */ 


#ifndef DRVBOOTLOAD_H_
#define DRVBOOTLOAD_H_

#include "../Conf/ConfHard.h"



//Init du bootload
void DrvBootloadAppInit( void ) ;
void DrvBootloadBootInit( void ) ;
void DrvBootloadDispatcher( void );

void DrvBootloadBootProgramPage (Int32U page, Int8U *buf);
char DrvBootloadFlashReadByte (Int32U addr);
void DrvBootloadFlashReadPage (Int8U page, Int8U *buf);


#endif /* DRVBOOTLOAD_H_ */