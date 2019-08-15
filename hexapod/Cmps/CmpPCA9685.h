/*
 * CmpPCA9685.h
 *
 * Created: 11/02/2014 12:00:20
 *  Author: berryer
 */ 


#ifndef CMPPCA9685_H_
#define CMPPCA9685_H_

#include "../Conf/ConfHard.h"

////////////////////////////////////////PUBLIC DEFINES/////////////////////////////////////////
#define PCA9685_ADDRESS_0						0x40
#define PCA9685_ADDRESS_1						0x41

#define PCA9685_SUBADR1 0x2
#define PCA9685_SUBADR2 0x3
#define PCA9685_SUBADR3 0x4

#define PCA9685_MODE1 0x0
#define PCA9685_PRESCALE 0xFE

#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9

#define ALLLED_ON_L 0xFA
#define ALLLED_ON_H 0xFB
#define ALLLED_OFF_L 0xFC
#define ALLLED_OFF_H 0xFD


////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////


////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////

Boolean CmpPCA9685Init( Int8U addr );
Boolean CmpPCA9685Reset( Int8U addr );
Boolean CmpPCA9685SetPWMFreq(Int8U addr, float freq);
Boolean CmpPCA9685SetPWM(Int8U addr, Int8U num, Int16U on, Int16U off);
Boolean CmpPCA9685SetPin(Int8U addr, Int8U num, Int16U val, Boolean invert);



#endif /* CMPPCA9685_H_ */