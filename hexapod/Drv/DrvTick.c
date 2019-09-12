/*
 * DrvTick.c
 *
 * Created: 03/07/2012 11:56:56
 *  Author: berryer
 */ 
#include "../Conf/ConfHard.h"

#include "DrvTick.h"

////////////////////////////////////////PRIVATE DEFINES/////////////////////////////////////////
#define TIMER0_TICKS_PER_US		2U //	1/16000000MHz * 8 => 2 ticks pour 1 �s

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
static volatile Int32U tickCounter = 0UL;

///////////////////////////////////////////PUBLIC FUNCTIONS////////////////////////////////////////
//on init le driver
Boolean DrvTickInit(void)
{	
	tickCounter = 0UL;
	//on init le timer
	TCCR0A	= 0U;
	TCNT0	= 0U;
	//normal mode
	BIT_LOW(TCCR0A,WGM00);
	BIT_LOW(TCCR0A,WGM01);
	BIT_LOW(TCCR0B,WGM02);
	//prescaler 8 * 1/Clk => 0,0000005 s => 0.5�s
	BIT_HIGH(TCCR0B,CS01);
	BIT_LOW(TCCR0B,CS00);
	BIT_HIGH(TIMSK0,TOIE0);
	
	return TRUE;
}

//get the tick counter in Us
Int32U DrvTickGetTimeUs(void)
{
	Int32U ovf0;
    Int8U tcnt0;
	
	ATOMIC(	
		ovf0 = tickCounter;
		tcnt0 = TCNT0;
		if ((TIFR0 & _BV(TOV0)) && (tcnt0 < 255))
		{
			ovf0++;
		}
	)
    return (Int32U)(((ovf0 << 8U) + tcnt0) / TIMER0_TICKS_PER_US);
}

//get the tick counter in Ms
Int32U DrvTickGetTimeMs(void)
{
	Int32U us = 0U;
	us = DrvTickGetTimeUs();
    return (Int32U)( us / 1000U) ;
}

//get the tick counter in seconds
Int32U DrvTickGetTimeSec(void)
{
	Int32U ms = 0U;
	ms = DrvTickGetTimeMs();
    return (Int32U)(ms / 1000U) ;
}


void DrvTickDelayMs( Int16U delay_ms )
{
	DrvTickDelayUs(delay_ms * 1000UL);
}


void DrvTickDelayUs( Int32U delay_us )
{
	Int32U timeToWait = 0UL;
	timeToWait = (Int32U)(DrvTickGetTimeUs() + (Int32U)delay_us);
	do 
	{
		if(DrvTickGetTimeUs() >= timeToWait)
		{
			break;
		}
	} while (TRUE);
}


///////////////////////////////////////////ISR FUNCTIONS//////////////////////////////////////////
//ISR timer system 0.5�s * 256 =  0.128 ms
ISR(TIMER0_OVF_vect) 
{
	tickCounter += 1UL ;
}

	