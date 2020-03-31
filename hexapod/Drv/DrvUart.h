/*
 * drv_uart.h
 *
 * Created: 28/06/2011 15:53:09
 *  Author: berryer
 */ 


#ifndef DRV_UART_H_
#define DRV_UART_H_

#include "../Conf/ConfHard.h"


////////////////////////////////////////////PUBLIC DEFINES/////////////////////////////////////////

//////////////////////////////////////////PUBLIC PTR FCTS//////////////////////////////////////////
//callback fire when data is received
typedef void (*DrvUartCallbackReceiveDataISR) (Int8U data);

/////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////

////////////////////////////////////////////PUBLIC ENUMS///////////////////////////////////////////

/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
// Init du Drv Uart 
Boolean DrvUartInit( void ) ;

Boolean DrvUartSetBaudRate(Int8U indexUart, Int32U baud);
Int8U DrvUartDataUsedTXBuff(Int8U indexUart);
Int8U DrvUartDataAvailable(Int8U indexUart);
Boolean DrvUartFillTxBuffer(Int8U indexUart, Int8U datum);

//envoie des data sur la liaison serie
Boolean DrvUartSendData(Int8U indexUart);

//recoit une donn�e de la liaison s�rie
Int8U DrvUartReadData(Int8U indexUart);
#endif /* DRV_UART_H_ */

