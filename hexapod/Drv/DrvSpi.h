/*
 * drv_uart.h
 *
 * Created: 28/06/2011 15:53:09
 *  Author: berryer
 */ 


#ifndef DRV_SPI_H_
#define DRV_SPI_H_

#include "../Conf/ConfHard.h"


////////////////////////////////////////////PUBLIC DEFINES/////////////////////////////////////////


//////////////////////////////////////////PUBLIC PTR FCTS//////////////////////////////////////////
//callback fire when data is received
typedef void (*DrvSpiCallbackReceiveDataISR) (Int8U data);

/////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////

////////////////////////////////////////////PUBLIC ENUMS///////////////////////////////////////////


/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
// Init du Drv Spi 
Boolean DrvSpiInit( void ) ;

//envoie des data sur la liaison serie
Boolean DrvSpiSendData( uint8_t dataToSend );

//recoit une donnee de la liaison serie
Int8U DrvSpiReadData( void );
#endif /* DRV_SPI_H_ */

