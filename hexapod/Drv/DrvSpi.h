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

//set CS pin
Boolean DrvSpiSetCS( EIoPin pin );
//envoie et recoie des data sur la liaison SPI
Int8U DrvSpiSendData( Int8U dataToSend );
//read register
Boolean DrvSpiReadReg( Int8U slave_register, Int8U *data ) ;
//write register
Boolean DrvSpiWriteReg( Int8U slave_register, Int8U data ) ;
//read many register
Boolean DrvSpiReadRegBuf(Int8U slave_register, Int8U *buffer, Int8U buffer_size) ;
//write many register
Boolean DrvSpiWriteRegBuf(Int8U slave_register, Int8U *buffer, Int8U buffer_size) ;

#endif /* DRV_SPI_H_ */

