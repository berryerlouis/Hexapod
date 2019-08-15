/*
 * DrvPwmHard.c
 *
 * Created: 23/10/2016 12:58:07
 *  Author: berryer
 */ 
////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"
#include "Drv/DrvPwmHard.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
//compute convert

#define ConvertUsToTicks(us)		(us / PWM_HARD_TICKS_PER_US)
#define ConvertTicksToUs(ticks)		(ticks * PWM_HARD_TICKS_PER_US)

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
typedef struct {
	Boolean enable;
	EIoPin pin;
	Int16U ticks;
	Int16U min;
	Int16U mid;
	Int16U max;
} PwmHard;

EIoPin pwmHardPins[ E_NB_HARD_PWMS ] = HARD_PWM_PINS;
PwmHard pwmsHard[ E_NB_HARD_PWMS ];
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////


/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
// Init of Drv PWM
Boolean DrvPwmHardInit( Int16U period )
{
	//init the struture to the dafault value
	for(Int8U indexPwmHard = 0U; indexPwmHard < E_NB_HARD_PWMS ; indexPwmHard++)
	{
		pwmsHard[ indexPwmHard ].enable = FALSE;
		pwmsHard[ indexPwmHard ].pin = pwmHardPins[ indexPwmHard ];
		pwmsHard[ indexPwmHard ].ticks = 0U;
		pwmsHard[ indexPwmHard ].min = 0U;
		pwmsHard[ indexPwmHard ].mid = 0U;
		pwmsHard[ indexPwmHard ].max = 0U;
		DrvIoSetPinOutput( pwmsHard[ indexPwmHard ].pin );
	}
	
	//set register to use OC1A et OC1B as output
	BIT_LOW (TCCR1A,COM1A0);	//clear OC1A on match
	BIT_HIGH(TCCR1A,COM1A1);	//clear OC1A on match
	BIT_LOW (TCCR1A,COM1B0);	//clear OC1B on match
	BIT_HIGH(TCCR1A,COM1B1);	//clear OC1B on match
	
	//set the mode of the counter as fast PWM on 8bit
	//the accuracy will be 255 steps
	BIT_HIGH(TCCR1A,WGM10);		//fast PWM 8 bit
	BIT_LOW (TCCR1A,WGM11);		//fast PWM 8 bit
	BIT_HIGH(TCCR1B,WGM12);		//fast PWM 8 bit
	BIT_LOW (TCCR1B,WGM13);		//fast PWM 8 bit
	
	//set the frequence
	//it will be clk/1024 = 16000000/1024 = 15625 Hz => 0.000064 s per step
	//the max period of the PWM will be 255 * 0.000064 = 0.01632 s
	BIT_HIGH(TCCR1B,CS10);		//clk/1024
	BIT_LOW (TCCR1B,CS11);		//clk/1024
	BIT_HIGH(TCCR1B,CS12);		//clk/1024
	
	OCR1A = 0U;					//reset
	OCR1B = 0U;					//reset
	
	return TRUE;
}


// enable or disable the selected PWM
Boolean DrvPwmHardSetEnable( Int8U index, Boolean enable )
{
	//index correspond to an PWM
	if( index < E_NB_HARD_PWMS )
	{
		pwmsHard[ index ].enable = enable;
		return TRUE;
	}
	return FALSE;
}
// set the width of the selected PWM
Boolean DrvPwmHardSetWidth( Int8U index, Int16U width )
{
	//index correspond to an PWM
	if( index < E_NB_HARD_PWMS )
	{
		pwmsHard[ index ].ticks = SetLimitsInt16U(ConvertUsToTicks(width),pwmsHard[ index ].min,pwmsHard[ index ].max);
		return TRUE;
	}
	return FALSE;
}
// set the minimum width of the selected PWM
Boolean DrvPwmHardSetWidthMin( Int8U index, Int16U width )
{
	//index correspond to an PWM
	if( index < E_NB_HARD_PWMS )
	{
		pwmsHard[ index ].min = ConvertUsToTicks(width);
		return TRUE;
	}
	return FALSE;
}
// set the maximum width of the selected PWM
Boolean DrvPwmHardSetWidthMax( Int8U index, Int16U width )
{
	//index correspond to an PWM
	if( index < E_NB_HARD_PWMS )
	{
		pwmsHard[ index ].max = ConvertUsToTicks(width);
		return TRUE;
	}
	return FALSE;
}
// set the default width of the selected PWM
Boolean DrvPwmHardSetWidthMid( Int8U index, Int16U width )
{
	//index correspond to an PWM
	if( index < E_NB_HARD_PWMS )
	{
		pwmsHard[ index ].mid = ConvertUsToTicks(width);
		return TRUE;
	}
	return FALSE;
}
// get the width of the selected PWM
Int16U DrvPwmHardGetWidth( Int8U index )
{
	if( index < E_NB_HARD_PWMS )
	{
		return pwmsHard[ index ].ticks;
	}
	return FALSE;
}
// get the PWM strucutre
PwmHard* DrvPwmHardGetStruture( Int8U index )
{
	if( index < E_NB_HARD_PWMS )
	{
		return &pwmsHard[ index ];
	}
	return NULL;
}
