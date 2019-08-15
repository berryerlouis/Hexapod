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

#define UART_PINS					{\
											UART_0_PINS, /*E_UART_0			*/\
											UART_1_PINS  /*E_UART_1			*/\
}

#define ComputeBaudRate( baud )				(Int16U)( CONF_FOSC_HZ / ( (Int32U)baud * CONF_FOSC_MHZ ) - 1 )
#define ComputeBaudRateDoubleSpeed( baud )	(Int16U)( CONF_FOSC_HZ / ( (Int32U)baud * (CONF_FOSC_MHZ/2) ) - 1 )

//////////////////////////////////////////PUBLIC PTR FCTS//////////////////////////////////////////
//callback fire when data is received
typedef void (*DrvUartCallbackReceiveDataISR) (Int8U data);

/////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////

////////////////////////////////////////////PUBLIC ENUMS///////////////////////////////////////////


typedef enum
{
	UART_SPEED_9600,
	UART_SPEED_38400,
	UART_SPEED_57600,
	UART_SPEED_76800,
	UART_SPEED_115200,
}UartBaudRate;



/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
// Init du Drv Uart 
Boolean DrvUartInit( Int8U indexUart, UartBaudRate baudRate ) ;


Int8U DrvUartDataUsedTXBuff(Int8U indexUart);
Int8U DrvUartDataAvailable(Int8U indexUart);
Boolean DrvUartFillTxBuffer(Int8U indexUart, Int8U datum);

//envoie des data sur la liaison serie
Boolean DrvUartSendData(Int8U indexUart);

//recoit une donn�e de la liaison s�rie
Int8U DrvUartReadData(Int8U indexUart);
#endif /* DRV_UART_H_ */

