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
#include "Drv/DrvEeprom.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////

 /////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
Boolean CmpPCA9685Init( Int8U addr )
{
	Boolean oSuccess = FALSE;
		
	oSuccess = CmpPCA9685Reset(addr);
	return oSuccess;
}

Boolean CmpPCA9685Reset( Int8U addr )
{
	return DrvTwiWriteReg( addr, PCA9685_MODE1, 0x80U );
}

Boolean CmpPCA9685SetPWMFreq(Int8U addr, float freq)
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
	return DrvTwiWriteReg(addr,PCA9685_MODE1, oldmode | 0xA0U);  //  This sets the MODE1 register to turn on auto increment.
}


Boolean CmpPCA9685SetPWM(Int8U addr, Int8U num, Int16U on, Int16U off)
{
	Int8U buffer [] = {	on,on>>8U,
						off, off>>8U};
	return DrvTwiWriteRegBuf(addr, LED0_ON_L + 4 * num, buffer, 4U);
}

Boolean CmpPCA9685SetAllPWM(Int8U addr, SPCA9685Pwm *pwm, Int8U nbPwm)
{						
	return DrvTwiWriteRegBuf(addr, LED0_ON_L, (Int8U*)pwm, nbPwm * sizeof(pwm[0]));
}

Boolean CmpPCA9685SetPin(Int8U addr, Int8U num, Int16U val, Boolean invert)
{
	// Clamp value between 0 and 4095 inclusive.
	val = MIN(val, (Int16U)4095);
	if (invert) {
		if (val == 0) {
			// Special value for signal fully on.
			return CmpPCA9685SetPWM(addr, num, 4096, 0);
		}
		else if (val == 4095) {
			// Special value for signal fully off.
			return CmpPCA9685SetPWM(addr, num, 0, 4096);
		}
		else {
			return CmpPCA9685SetPWM(addr, num, 0, 4095-val);
		}
	}
	else {
		if (val == 4095) {
			// Special value for signal fully on.
			return CmpPCA9685SetPWM(addr, num, 4096, 0);
		}
		else if (val == 0) {
			// Special value for signal fully off.
			return CmpPCA9685SetPWM(addr, num, 0, 4096);
		}
		else {
			return CmpPCA9685SetPWM(addr, num, 0, val);
		}
	}
	return FALSE;
}
