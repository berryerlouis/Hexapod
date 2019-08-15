/*
 * CmpSRF04.c
 *
 * Created: 03/07/2012 13:48:49
 *  Author: berryer
 */
/////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
#include "CmpSRF04.h"
 
#include "Drv/DrvIO.h"
#include "Drv/DrvTick.h"
#include "Drv/DrvTimer.h"
#include "Drv/DrvEeprom.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define SRF04_ECHO_TIMEOUT	30000U		//30ms
#define SRF04_TIMEOUT		30U		//30ms
////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
typedef enum
{
	SEND_PULSE,
	WAIT_RISING_EDGE,
	WAIT_FALLING_EDGE,
}E_StatusSRF04;

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
volatile E_StatusSRF04 statusEcho = SEND_PULSE;
volatile Int32U timeBeforeEchoStart = 0UL;
volatile Int16U timeoutEcho = 0UL;

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
static Boolean CmpSRF04Init( void );
static Boolean CmpSRF04SendPulse( void );

//definition de la structure des fonctions du ultrasonic pour le composant SRF04
UltraSonicFunctions srf04 =
{
	CmpSRF04Init,
	CmpSRF04SendPulse,
};

//definition de la structure des data ultrasonic pour le composant SRF04
UltraSonicData srf04Data;
EIoPin srf04Pins[ E_NB_USS ] = US_PINS ;

/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////

//fonction init du capteur
static Boolean CmpSRF04Init( void )
{
	DrvIoSetPinOutput(srf04Pins[0U]);
	DrvIoSetPinState(srf04Pins[0U],IO_LEVEL_LOW);
	statusEcho = SEND_PULSE;
	timeoutEcho = 0U;
	return TRUE;
}

// send a US pulse
static Boolean CmpSRF04SendPulse( void )
{
	Boolean oSuccess = FALSE;
	if(statusEcho == SEND_PULSE)
	{
		//send a pulse 
		DrvIoSetPinOutput(srf04Pins[0U]);
		DrvIoSetPinState(srf04Pins[0U],IO_LEVEL_HIGH);
		DrvTickDelayUs(10);
		DrvIoSetPinState(srf04Pins[0U],IO_LEVEL_LOW);
		//wait for response from the SRF04
		DrvIoSetPinInput(srf04Pins[0U]);
		
		//update timeout and status
		timeoutEcho = 0U;
		statusEcho = WAIT_RISING_EDGE;
		
		//enable ext int
		DrvIoSetInterruptPin(srf04Pins[0U]);
	}
	else
	{
		//count a timeout if detecting issue
		if(++timeoutEcho >= SRF04_TIMEOUT)
		{
			statusEcho = SEND_PULSE;
			timeoutEcho = 0U;
		}
	}
	return oSuccess;
}

/////////////////////////////////////ISR PRIVATE FUNCTIONS////////////////////////////////////////
//ISR pin Sonar
ISR(PCINT0_vect)
{
	Int32U time = DrvTickGetTimeUs();
	if(statusEcho == WAIT_RISING_EDGE)
	{
		timeBeforeEchoStart = time;
		statusEcho = WAIT_FALLING_EDGE;
	}
	else
	{
		Int32U timeAfterEcho = time - timeBeforeEchoStart;
		//must be inf to 30 ms
		if( timeAfterEcho <  SRF04_ECHO_TIMEOUT)
		{
			srf04Data.distance = timeAfterEcho / 58UL;
		}
		statusEcho = SEND_PULSE;
		
		//disable ext int
		DrvIoResetInterruptPin(srf04Pins[0U]);
	}
}