/*
 * CmpSRF04.c
 *
 * Created: 03/07/2012 13:48:49
 *  Author: berryer
 */
/////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
#include "CmpSRF04.h"
#include "Drv/DrvTick.h"
 

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define SRF04_ECHO_TIMEOUT	50000U		//30ms
#define SRF04_TIMEOUT		10U		//30ms
////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
typedef enum
{
	SEND_PULSE,
	WAIT_RISING_EDGE,
	WAIT_FALLING_EDGE
}E_StatusSRF04;

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
volatile E_StatusSRF04 statusEcho[ E_NB_USS ] = {SEND_PULSE};
volatile Int32U timeBeforeEchoStart[ E_NB_USS ] = {0UL};
volatile Int16U timeoutEcho[ E_NB_USS ] = {0UL};

EIoPin srf04PinEcho[ E_NB_USS ] = US_PINS_ECHO ;
EIoPin srf04PinPulse[ E_NB_USS ] = US_PINS_PULSE ;
Int16U distance[ E_NB_USS ] = {0U};
EIOLevel levelPin[ E_NB_USS ];

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////

//fonction init du capteur
Boolean CmpSRF04Init( void )
{
	for(uint8_t loop_us = 0U ; loop_us < E_NB_USS ; loop_us++)
	{
		//set pin to output
		DrvIoSetPinOutput(srf04PinPulse[loop_us]);
		DrvIoSetPinState(srf04PinPulse[loop_us],IO_LEVEL_LOW);
		
		//set pin to input
		DrvIoSetPinInput(srf04PinEcho[loop_us]);
		//enable ext int
		DrvIoSetInterruptPin(srf04PinEcho[loop_us]);
		
		statusEcho[loop_us] = SEND_PULSE;
		timeoutEcho[loop_us] = 0U;
		timeBeforeEchoStart[loop_us] = 0U;
	}
	
	return TRUE;
}

// send a US pulse
Boolean CmpSRF04SendPulse( E_US us )
{
	Boolean oSuccess = FALSE;
	
	if(statusEcho[us] == SEND_PULSE)
	{
		//send a pulse
		DrvIoSetPinState(srf04PinPulse[us],IO_LEVEL_HIGH);
		DrvTickDelayUs(10);
		DrvIoSetPinState(srf04PinPulse[us],IO_LEVEL_LOW);
			
		//update timeout and status
		timeoutEcho[us] = 0U;
		statusEcho[us] = WAIT_RISING_EDGE;
			
	}
	else
	{
		//count a timeout if detecting issue
		if(++timeoutEcho[us] >= SRF04_TIMEOUT)
		{
			statusEcho[us] = SEND_PULSE;
			timeoutEcho[us] = 0U;
		}
	}
	
	return oSuccess;
}

Int16U CmpSRF04GetDistance( E_US us )
{
	return distance[us];
}
/////////////////////////////////////ISR PRIVATE FUNCTIONS////////////////////////////////////////
//ISR pin Sonar
ISR(PCINT0_vect)
{
	for(uint8_t loop_us = 0U ; loop_us < E_NB_USS ; loop_us++)
	{
		levelPin[loop_us] = DrvIoGetPinState(srf04PinEcho[loop_us]);
		Int32U time = DrvTickGetTimeUs();
		if(levelPin[loop_us] == IO_LEVEL_HIGH)
		{
			if(statusEcho[loop_us] == WAIT_RISING_EDGE)
			{
				timeBeforeEchoStart[loop_us] = time;
				statusEcho[loop_us] = WAIT_FALLING_EDGE;
			}
		}
		else
		{
			Int32U timeAfterEcho = time - timeBeforeEchoStart[loop_us];
			//must be inf to 30 ms
			if( timeAfterEcho < SRF04_ECHO_TIMEOUT)
			{
				distance[loop_us] = timeAfterEcho / 58UL;
			}
			statusEcho[loop_us] = SEND_PULSE;
			timeoutEcho[loop_us] = 0U;
			timeBeforeEchoStart[loop_us] = 0U;
		}
	}
}