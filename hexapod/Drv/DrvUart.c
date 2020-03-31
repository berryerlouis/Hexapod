/*
 * drv_uart.c
 *
 * Created: 28/06/2011 15:52:35
 *  Author: berryer
 */ 

#include "../Conf/ConfHard.h"

#include "DrvUart.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define MAX_BUFFER	512U

#define ComputeBaudRate( baud )				( CONF_FOSC_HZ / ( baud * 16 ))  
#define ComputeBaudRateDoubleSpeed( baud )	( CONF_FOSC_HZ / ( baud * (16/2))) - 1 


////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////
typedef struct
{
	Int8U Head;
	Int8U Tail;
	Int8U Buffer[ MAX_BUFFER ];
}RingBuffer ;

typedef struct
{
	RingBuffer Rx;
	RingBuffer Tx;
}UartRingBuffer ;


////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
static volatile UartRingBuffer uartRingBuffer[ E_NB_UARTS ];
EIoPin uartPins[ E_NB_UARTS][2U] = {UART_0_PINS} ;
	
/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
// Init du Drv Uart 
Boolean DrvUartInit( void )
{
	Boolean oSuccess = FALSE;
	#if E_UART_0 == 0U
	
		DrvIoSetPinOutput(uartPins[ E_UART_0 ][UART_TX_PIN]);
		DrvIoSetPinInput(uartPins[ E_UART_0 ][UART_RX_PIN]);
	
		#ifdef E_UART_0_BAUD_RATE
			UBRR0 = ComputeBaudRateDoubleSpeed(E_UART_0_BAUD_RATE);	
		#else
			UBRR0 = ComputeBaudRateDoubleSpeed(9600);
		#endif
		BIT_HIGH(UCSR0A,U2X0);		//double speed mode
		BIT_HIGH(UCSR0B,RXEN0);		//enable RX
		BIT_HIGH(UCSR0B,TXEN0);		//enable TX
		BIT_HIGH(UCSR0C,UCSZ00);	//8 bits, no parity, 1 stop
		BIT_LOW(UCSR0B,TXCIE0);		//disable TX interrupt
		BIT_LOW(UCSR0B,UDRIE0);		//disable UDR interrupt
		BIT_HIGH(UCSR0B,RXCIE0);	//enable RX interrupt
		oSuccess = TRUE;
	#endif
	
	#ifdef E_UART_1
	
		DrvIoSetPinOutput(i2cPins[ E_UART_1 ][UART_TX_PIN]);
		DrvIoSetPinInput(i2cPins[ E_UART_1 ][UART_RX_PIN]);
		
		#ifdef E_UART_1_BAUD_RATE
			UBRR1 = ComputeBaudRate(E_UART_1_BAUD_RATE);
		#else
			UBRR1 = ComputeBaudRate(9600);
		#endif
		//BIT_HIGH(UCSR1A,U2X1);		//double speed mode
		BIT_HIGH(UCSR1B,RXEN1);		//enable RX
		BIT_HIGH(UCSR1B,TXEN1);		//enable TX
		BIT_HIGH(UCSR1C,UCSZ10);	//8 bits, no parity, 1 stop
		BIT_HIGH(UCSR1B,RXCIE1);	//enable RX interrupt
		BIT_LOW(UCSR1B,TXCIE1);		//disable TX interrupt
		BIT_LOW(UCSR1B,UDRIE1);		//disable UDR interrupt
		BIT_HIGH(UCSR1B,RXCIE1);	//enable RX interrupt
		oSuccess = TRUE;
	#endif
	return oSuccess;
}

Boolean DrvUartSetBaudRate(Int8U indexUart, Int32U baud)
{
	#if E_UART_0 == 0U
		BIT_LOW(UCSR0B,RXEN0);		//disable RX
		BIT_LOW(UCSR0B,TXEN0);		//disable TX
		BIT_LOW(UCSR0B,RXCIE0);	//disable RX interrupt
		UBRR0 = ComputeBaudRateDoubleSpeed(baud);
		BIT_HIGH(UCSR0B,RXEN0);		//enable RX
		BIT_HIGH(UCSR0B,TXEN0);		//enable TX
		BIT_HIGH(UCSR0B,RXCIE0);	//enable RX interrupt
	#endif
	
	#ifdef E_UART_1
		BIT_LOW(UCSR1B,RXEN1);		//disable RX
		BIT_LOW(UCSR1B,TXEN1);		//disable TX
		BIT_LOW(UCSR1B,RXCIE1);	//disable RX interrupt
		UBRR1 = ComputeBaudRate(baud);
		BIT_HIGH(UCSR1B,RXEN1);		//enable RX
		BIT_HIGH(UCSR1B,TXEN1);		//enable TX
		BIT_HIGH(UCSR1B,RXCIE1);	//enable RX interrupt
	#endif
}

//rempli le buffer d'emission
Boolean DrvUartFillTxBuffer(Int8U indexUart, Int8U datum)
{
	Int8U head = uartRingBuffer[indexUart].Tx.Head;
	if (++head >= MAX_BUFFER)
	{
		head = 0U;
	}
	uartRingBuffer[indexUart].Tx.Buffer[head] = datum;
	uartRingBuffer[indexUart].Tx.Head = head;
	
	return TRUE;
}

//envoie des données sur la liaison série
Boolean DrvUartSendData(Int8U indexUart)
{
	Boolean oSuccess = FALSE;
	if(indexUart == E_UART_0)
	{
		//enable ISR UDRIE0
		BIT_HIGH(UCSR0B,UDRIE0);
		oSuccess = TRUE;
	}
	else if(indexUart == E_UART_0)
	{
		#ifdef UCSR1B
		//enable ISR UDRIE1
		BIT_HIGH(UCSR1B,UDRIE1);
		oSuccess = TRUE;
		#endif
	}
	return oSuccess;
}

//recoit une donn�e de la liaison s�rie
Int8U DrvUartReadData(Int8U indexUart)
{
	Int8U tail = uartRingBuffer[indexUart].Rx.Tail;
	Int8U datum = uartRingBuffer[indexUart].Rx.Buffer[tail];
	if (uartRingBuffer[indexUart].Rx.Head != tail)
	{
		if (++tail >= MAX_BUFFER)
		{
			tail= 0U;
		}
		uartRingBuffer[indexUart].Rx.Tail = tail;
	}
	return datum;
}

Int8U DrvUartDataAvailable(Int8U indexUart)
{
	Int8U nbDataAvailable = uartRingBuffer[indexUart].Rx.Head - uartRingBuffer[indexUart].Rx.Tail;
	return nbDataAvailable % MAX_BUFFER;
}

Int8U DrvUartDataUsedTXBuff(Int8U indexUart) 
{
	Int8U nbDataUsed = uartRingBuffer[indexUart].Tx.Head - uartRingBuffer[indexUart].Tx.Tail;
	return nbDataUsed % MAX_BUFFER;
}


////////////////////////////////////////ISR FUNCTIONS/////////////////////////////////////////

#ifdef USART0_RX_vect
#ifdef USART0_RX_vect
	ISR(USART0_RX_vect)
#endif
#ifdef USART_RX_vect
	ISR(USART_RX_vect)
#endif
{
	Int8U head = uartRingBuffer[E_UART_0].Rx.Head;
	uartRingBuffer[E_UART_0].Rx.Buffer[head++] = UDR0;
	if (head >= MAX_BUFFER)
	{
		head = 0U;
	}
	uartRingBuffer[E_UART_0].Rx.Head = head;
}

#ifdef USART_UDRE_vect
ISR(USART_UDRE_vect)
#endif
#ifdef USART0_UDRE_vect
ISR(USART0_UDRE_vect)
#endif
{
	Int8U tail = uartRingBuffer[E_UART_0].Tx.Tail;
	if (uartRingBuffer[E_UART_0].Tx.Head != tail)
	{
		if (++tail >= MAX_BUFFER)
		{
			tail = 0U;
		}
		UDR0 = uartRingBuffer[E_UART_0].Tx.Buffer[tail];
		uartRingBuffer[E_UART_0].Tx.Tail = tail;
	}
	if (tail == uartRingBuffer[E_UART_0].Tx.Head)
	{
		BIT_LOW(UCSR0B,UDRIE0);
	}
}

#ifdef USART_TX_vect
ISR(USART_TX_vect)
#endif
#ifdef USART0_TX_vect
ISR(USART0_TX_vect)
#endif
{

}
#endif // UART_1_PINS


#ifdef UDR1
ISR(USART1_RX_vect)
{
	Int8U head = uartRingBuffer[E_UART_0].Rx.Head;
	uartRingBuffer[E_UART_0].Rx.Buffer[head++] = UDR1;
	if (head >= MAX_BUFFER)
	{
		head = 0U;
	}
	uartRingBuffer[E_UART_0].Rx.Head = head;
}

ISR(USART1_UDRE_vect)
{
	Int8U tail = uartRingBuffer[E_UART_0].Tx.Tail;
	if (uartRingBuffer[E_UART_0].Tx.Head != tail)
	{
		if (++tail >= MAX_BUFFER)
		{
			tail = 0U;
		}
		UDR1 = uartRingBuffer[E_UART_0].Tx.Buffer[tail];
		uartRingBuffer[E_UART_0].Tx.Tail = tail;
	}
	if (tail == uartRingBuffer[E_UART_0].Tx.Head)
	{
		BIT_LOW(UCSR1B,UDRIE1);	
	}
}

ISR(USART1_TX_vect)
{

}
#endif // UART_2_PINS