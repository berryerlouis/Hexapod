/*
 * DrvPwmSoft.c
 *
 * Created: 28/06/2011 15:52:35
 *  Author: berryer
 */ 
////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf\ConfHard.h"
#include "Drv/DrvPwmSoft.h"


////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define OFFSET_TIMING	8U
//compute convert


#define ConvertUsToTicks(us)		(us * PWM_SOFT_TICKS_PER_US)
#define ConvertTicksToUs(ticks)		(ticks / PWM_SOFT_TICKS_PER_US)


////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
//index du servo controllé lors de l'IT
volatile Int8U idPwmSoft = 0U;
static Int16U periodPwmSoft = 0U;

EIoPin pwmSoftPins[ E_NB_SOFT_PWMS ] = SOFT_PWM_PINS ;
PwmSoft pwmsSoft[ E_NB_SOFT_PWMS ];
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////


/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////


// Init of Drv PWM
Boolean DrvPwmSoftInit( Int16U period ) 
{
	//init the struture to the dafault value
	for(Int8U indexPwmSoft = 0U; indexPwmSoft < E_NB_SOFT_PWMS ; indexPwmSoft++)
	{
		pwmsSoft[ indexPwmSoft ].enable = FALSE;
		pwmsSoft[ indexPwmSoft ].needUpdate = FALSE;
		pwmsSoft[ indexPwmSoft ].pin = pwmSoftPins[ indexPwmSoft ];
		pwmsSoft[ indexPwmSoft ].ticks = 0U;
		pwmsSoft[ indexPwmSoft ].tempTicks = 0U;
		pwmsSoft[ indexPwmSoft ].min = 0U;
		pwmsSoft[ indexPwmSoft ].mid = 0U;
		pwmsSoft[ indexPwmSoft ].max = 0U;
		DrvIoSetPinOutput( pwmsSoft[ indexPwmSoft ].pin );
	}
	
	//set idx to 0 at startup
	idPwmSoft = 0U;
	
	//on init le timer 1 tick = 0.5us
	BIT_LOW(TCCR1A,WGM11); //mode normal
	BIT_LOW(TCCR1A,WGM10); //mode normal
	BIT_LOW(TCCR1B,WGM13); //mode normal
	BIT_LOW(TCCR1B,WGM12); //mode normal
	
	BIT_LOW(TIMSK1,OCIE1A);//disable ISR on OCIE1A
	BIT_LOW(TIMSK1,OCIE1B);//disable ISR on OCIE1B
	BIT_LOW(TIMSK1,TOIE1); //disable ISR on TOIE1
	BIT_LOW(TIMSK1,ICIE1); //disable ISR on ICIE1
	
	//set period
	DrvPwmSoftSetPeriod(period);
	
	//reset counter to 0
	TCNT1	= 0x0000U;
	return TRUE;
}


// set the period of all PWM
Boolean DrvPwmSoftSetPeriod( Int16U period ) 
{
	periodPwmSoft = ConvertUsToTicks(period);
	if(periodPwmSoft != 0U)
	{
		OCR1B = periodPwmSoft;
		BIT_LOW (TCCR1B,CS12);	//prescaler 8
		BIT_HIGH(TCCR1B,CS11);	//prescaler 8
		BIT_LOW (TCCR1B,CS10);	//prescaler 8
		BIT_HIGH(TIMSK1,OCIE1B);//enable ISR on OCIE1B
	}
	else
	{
		OCR1B = 0U;
		BIT_LOW (TCCR1B,CS12); //disable
		BIT_LOW (TCCR1B,CS11); //disable
		BIT_LOW (TCCR1B,CS10); //disable
		BIT_LOW(TIMSK1,OCIE1B);//disable ISR on OCIE1B
	}
	return TRUE;
}

// enable or disable the selected PWM
Boolean DrvPwmSoftSetEnable( Int8U index, Boolean enable ) 
{
	//index correspond to an PWM
	if( index < E_NB_SOFT_PWMS )
	{
		pwmsSoft[ index ].enable = enable;
		return TRUE;
	}
	return FALSE;
}
// set the width of the selected PWM
Boolean DrvPwmSoftSetWidth( Int8U index, Int16U width ) 
{	
	//index correspond to an PWM
	if( index < E_NB_SOFT_PWMS )
	{
		//accept changes
		pwmsSoft[ index ].tempTicks = ConvertUsToTicks(SetLimitsInt16U(width,pwmsSoft[ index ].min,pwmsSoft[ index ].max));
		pwmsSoft[ index ].needUpdate = TRUE;
	}
	return FALSE;
}
// set the minimum width of the selected PWM
Boolean DrvPwmSoftSetWidthMin( Int8U index, Int16U width )
{
	//index correspond to an PWM
	if( index < E_NB_SOFT_PWMS )
	{
		pwmsSoft[ index ].min = width;
		return TRUE;
	}
	return FALSE;
}
// set the maximum width of the selected PWM
Boolean DrvPwmSoftSetWidthMax( Int8U index, Int16U width )
{
	//index correspond to an PWM
	if( index < E_NB_SOFT_PWMS )
	{
		pwmsSoft[ index ].max = width;
		return TRUE;
	}
	return FALSE;
}
// set the default width of the selected PWM
Boolean DrvPwmSoftSetWidthMid( Int8U index, Int16U width )
{
	//index correspond to an PWM
	if( index < E_NB_SOFT_PWMS )
	{
		pwmsSoft[ index ].mid = width;
		return TRUE;
	}
	return FALSE;
}
// get the width of the selected PWM
Int16U DrvPwmSoftGetWidth( Int8U index ) 
{
	if( index < E_NB_SOFT_PWMS )
	{
		return pwmsSoft[ index ].ticks;
	}
	return FALSE;
}
// get the PWM strucutre
PwmSoft* DrvPwmSoftGetStruture( Int8U index ) 
{
	if( index < E_NB_SOFT_PWMS )
	{
		return &pwmsSoft[ index ];
	}
	return NULL;
}
 

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

///////////////////////////////////////////ISR FUNCTIONS//////////////////////////////////////////
// this IT permits to set the period of the all servos 
SIGNAL (TIMER1_COMPB_vect)
{		
	//id servo set to zero
	idPwmSoft = 0U;
	
	//set the next OCR1A to start the first servo PWM
	OCR1A = TCNT1 + OFFSET_TIMING;
	//servo period
	OCR1B = TCNT1 + periodPwmSoft + OFFSET_TIMING;
			
	//clear flag
	BIT_HIGH(TIFR1,OCF1A);
	//enable ISR on OCIE1A
	BIT_HIGH(TIMSK1,OCIE1A);
}


// this IT permits to set the width of each servos 
SIGNAL (TIMER1_COMPA_vect) 
{ 	
	if( (Int8S)(idPwmSoft) - 1 >= 0 )
	{
		//set servo pin to GND
		DrvIoSetPinState(pwmsSoft[ idPwmSoft - 1U ].pin, IO_LEVEL_LOW);
		//now we can update the tick
		if( pwmsSoft[ idPwmSoft - 1U ].needUpdate == TRUE )
		{
			pwmsSoft[ idPwmSoft - 1U ].ticks = pwmsSoft[ idPwmSoft - 1U ].tempTicks;
			pwmsSoft[ idPwmSoft - 1U ].needUpdate = FALSE;
		}
		
	}
	if( idPwmSoft < E_NB_SOFT_PWMS )
	{
		//if enable
		if(TRUE == pwmsSoft[ idPwmSoft ].enable)
		{
			//set first servo pin to VCC
			DrvIoSetPinState(pwmsSoft[ idPwmSoft ].pin, IO_LEVEL_HIGH);
		}
		//set the next OCR1A to the first servo tick
		OCR1A = pwmsSoft[ idPwmSoft ].ticks + TCNT1 + OFFSET_TIMING;
		//next servo
		idPwmSoft++;
	}
	else
	{
		//disable ISR on OCIE1A
		BIT_LOW(TIMSK1,OCIE1A);
	}
}
