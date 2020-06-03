/*
 * CmpPCA9685.h
 *
 * Created: 11/02/2014 12:00:20
 *  Author: berryer
 */ 


#ifndef CMPPCA9685_H_
#define CMPPCA9685_H_

#include "Conf/ConfHard.h"

////////////////////////////////////////PUBLIC DEFINES/////////////////////////////////////////
#define NB_PCA9685								2
#define PCA9685_ADDRESS_0						0x40
#define PCA9685_ADDRESS_1						0x41


#define PCA9685_PRESCALE	0xFE

#define PCA9685_MODE1			0x00
#define PCA9685_MODE1_SLEEP		0x10
#define PCA9685_MODE1_AI		0x20
#define PCA9685_MODE1_RESTART	0x80
#define PCA9685_MODE2			0x01
#define PCA9685_MODE2_OCH		0x04

#define PCA9685_NB_LEDS 16U

#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9

#define ALLLED_ON_L 0xFA
#define ALLLED_ON_H 0xFB
#define ALLLED_OFF_L 0xFC
#define ALLLED_OFF_H 0xFD


////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
//pwm structure representing the pwm of one output time on and off of PCA9685
typedef struct
{
	Int16U on;
	Int16U off;
}SPCA9685Pwm;

////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
 //component initialization
 Boolean CmpPCA9685Init( Int8U addr, float freq );

 //reset component
 Boolean CmpPCA9685Reset( Int8U addr );

 // get the pwm structure pointer at index
 SPCA9685Pwm* CmpPCA9685GetPwmTab( Int8U addr, Int8U index );

 //send current tab to component
 Boolean CmpPCA9685SendBuffer( Int8U addr, Int8U nbPwm );

//send selected pwm to component
Boolean CmpPCA9685Send( Int8U addr, Int8U idPwm, SPCA9685Pwm pwm );

#endif /* CMPPCA9685_H_ */