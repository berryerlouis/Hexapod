/*
 * drv_uart.c
 *
 * Created: 28/06/2011 15:52:35
 *  Author: berryer
 */ 

#include "../Conf/ConfHard.h"

#include "DrvUart.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define MAX_BUFFER	255U

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
	
/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
// Init du Drv Uart 
Boolean DrvUartInit( Int8U indexUart, UartBaudRate baudRate )
{
	Boolean oSuccess = TRUE;
	Int16U ubrr = 0U;
	switch (baudRate)
	{
	case UART_SPEED_9600: ubrr = ComputeBaudRateDoubleSpeed(9600);
		break;
	case UART_SPEED_38400: ubrr = ComputeBaudRateDoubleSpeed(38400);
		break;
	case UART_SPEED_57600: ubrr = ComputeBaudRateDoubleSpeed(57600);
		break;
	case UART_SPEED_76800: ubrr = ComputeBaudRateDoubleSpeed(76800);
		break;
	case UART_SPEED_115200: ubrr = ComputeBaudRate(115200);
		break;
		default:
	oSuccess = FALSE;
		break;
	}

	//ubrr is correct
	if(oSuccess == TRUE)
	{
		oSuccess = FALSE;
		if(indexUart == E_UART_0)
		{			
			UBRR0 = 0x000A;
			//BIT_HIGH(UCSR0A,U2X0);		//double speed mode
			BIT_HIGH(UCSR0B,RXEN0);		//enable RX
			BIT_HIGH(UCSR0B,TXEN0);		//enable TX
			BIT_HIGH(UCSR0C,UCSZ00);	//8 bits, no parity, 1 stop
			BIT_LOW(UCSR0B,TXCIE0);		//disable TX interrupt
			BIT_LOW(UCSR0B,UDRIE0);		//disable UDR interrupt
			BIT_HIGH(UCSR0B,RXCIE0);	//enable RX interrupt
						
			oSuccess = TRUE;
		}
		else if(indexUart == E_UART_0)
		{
			#ifdef UBRR1
			UBRR1 = ubrr;
			BIT_HIGH(UCSR1A,U2X1);		//double speed mode
			BIT_HIGH(UCSR1B,RXEN1);		//enable RX
			BIT_HIGH(UCSR1B,TXEN1);		//enable TX
			BIT_HIGH(UCSR1C,UCSZ10);	//8 bits, no parity, 1 stop
			BIT_HIGH(UCSR1B,RXCIE1);	//enable RX interrupt
			BIT_LOW(UCSR1B,TXCIE1);		//disable TX interrupt
			BIT_LOW(UCSR1B,UDRIE1);		//disable UDR interrupt
			BIT_HIGH(UCSR1B,RXCIE1);	//enable RX interrupt
			oSuccess = TRUE;
			#endif
		}
	}
	return oSuccess;
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

//envoie des donn�es sur la liaison s�rie
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
	return nbDataAvailable%MAX_BUFFER;
}

Int8U DrvUartDataUsedTXBuff(Int8U indexUart) 
{
	Int8U nbDataUsed = uartRingBuffer[indexUart].Tx.Head - uartRingBuffer[indexUart].Tx.Tail;
	return nbDataUsed%MAX_BUFFER;
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