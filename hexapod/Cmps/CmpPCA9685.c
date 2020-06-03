/*
 * CmpPCA9685.c
 *
 * Created: 11/02/2014 12:00:01
 *  Author: berryer
 */ 

/////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
#include "Cmps/CmpPCA9685.h"

#include "Drv/DrvTwi.h"
#include "Drv/DrvTick.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
//set the driving PWM frequency for outputs
static Boolean CmpPCA9685SetPWMFreq(Int8U addr, float freq);

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
//structure of PCA9585 tab representing pwms for each output
SPCA9685Pwm pwmOutpus[NB_PCA9685][PCA9685_NB_LEDS];

 /////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
 //component initialization
Boolean CmpPCA9685Init( Int8U addr, float freq )
{
	Boolean oSuccess = FALSE;
	//initialize tab of output with output low
	for (Int8U index = 0U; index < PCA9685_NB_LEDS ; index++)
	{
		pwmOutpus[addr == PCA9685_ADDRESS_0? 0 : 1][index].on = 0U;
		pwmOutpus[addr == PCA9685_ADDRESS_0? 0 : 1][index].off = 4096;
	}
	//reset component
	oSuccess = CmpPCA9685Reset(addr);
	//set driving frequency for pwm
	oSuccess = CmpPCA9685SetPWMFreq(addr,freq);
	//off to all pwm
	oSuccess = CmpPCA9685SendBuffer(addr,PCA9685_NB_LEDS);
	return oSuccess;
}

//reset component
Boolean CmpPCA9685Reset( Int8U addr )
{
	return DrvTwiWriteReg( addr, PCA9685_MODE1, PCA9685_MODE1_RESTART );
}

// get the pwm structure pointer at index
SPCA9685Pwm* CmpPCA9685GetPwmTab( Int8U addr, Int8U index )
{
	//check index
	if( index < PCA9685_NB_LEDS )
	{
		//return right pwm structure
		return &pwmOutpus[addr == PCA9685_ADDRESS_0? 0 : 1][ index ];
	}
	//return null if wrong
	return NULL;
}

//send the current tab to component
Boolean CmpPCA9685SendBuffer( Int8U addr, Int8U nbPwm )
{
	return DrvTwiWriteRegBuf(addr, LED0_ON_L, (Int8U*)pwmOutpus[addr == PCA9685_ADDRESS_0? 0 : 1], nbPwm * 4);
}

//send selected pwm to component
Boolean CmpPCA9685Send( Int8U addr, Int8U idPwm, SPCA9685Pwm pwm )
{
	Int8U bufferPwm [] = {	pwm.on,
						pwm.on >> 8U,
						pwm.off, 
						pwm.off >> 8U  };
	return DrvTwiWriteRegBuf(addr, LED0_ON_L + 4 * idPwm, bufferPwm, 4U);
}



/////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
//set the driving PWM frequency for outputs
static Boolean CmpPCA9685SetPWMFreq(Int8U addr, float freq)
{
	freq *= 0.9;  // Correct for overshoot in the frequency setting (see issue #11).
	float prescaleval = 25000000.0;
	prescaleval /= 4096.0;
	prescaleval /= freq;
	prescaleval -= 1.0;

	Int8U prescale = floor(prescaleval + 0.5);
	
	Int8U oldmode = 0U;

	DrvTwiReadReg(addr,PCA9685_MODE1, &oldmode );
	Int8U newmode = (oldmode & 0x7FU) | 0x10U; // sleep
	DrvTwiWriteReg(addr,PCA9685_MODE1, newmode); // go to sleep
	DrvTwiWriteReg(addr,PCA9685_PRESCALE, prescale); // set the prescaler
	DrvTwiWriteReg(addr,PCA9685_MODE1, oldmode);
	DrvTickDelayMs(1U);
	DrvTwiWriteReg(addr,PCA9685_MODE1, oldmode | PCA9685_MODE1_RESTART | PCA9685_MODE1_AI); //  This sets the MODE1 register to turn on auto increment.
	
	DrvTwiReadReg(addr,PCA9685_MODE2, &oldmode );
	DrvTwiWriteReg(addr,PCA9685_MODE2, oldmode | PCA9685_MODE2_OCH); 
	
	
	
	return TRUE;
}