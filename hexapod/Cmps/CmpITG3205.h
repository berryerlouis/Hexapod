/*
 * CmpITG3205.h
 *
 * Created: 03/07/2012 13:49:01
 *  Author: berryer
 */ 


#ifndef CMPITG3205_H_
#define CMPITG3205_H_



#include "Conf/conf_hard.h"

////////////////////////////////////////PUBLIC DEFINES/////////////////////////////////////////

#define ITG3205_TWI_ADDRESS				0x68

#define ITG3205_RA_WHO_AM_I				0x00
#define ITG3205_RA_SMPLRT_DIV			0x15
#define ITG3205_RA_DLPF_FS				0x16
#define ITG3205_RA_INT_CFG				0x17
#define ITG3205_RA_INT_STATUS			0x1A
#define ITG3205_RA_TEMP_OUT_H			0x1B
#define ITG3205_RA_TEMP_OUT_L			0x1C
#define ITG3205_RA_GYRO_XOUT_H			0x1D
#define ITG3205_RA_GYRO_XOUT_L			0x1E
#define ITG3205_RA_GYRO_YOUT_H			0x1F
#define ITG3205_RA_GYRO_YOUT_L			0x20
#define ITG3205_RA_GYRO_ZOUT_H			0x21
#define ITG3205_RA_GYRO_ZOUT_L			0x22
#define ITG3205_RA_PWR_MGM				0x3E

#define NO_SMPLRT_DIV					0x00

#define FS_RANGE_2000					0x18
#define ITG3205_PWR_H_RESET_BIT			0x80


#define LPFBW_256HZ						0x00
#define LPFBW_188HZ						0x01
#define LPFBW_98HZ						0x02
#define LPFBW_42HZ						0x03
#define LPFBW_20HZ						0x04
#define LPFBW_10HZ						0x05
#define LPFBW_5HZ						0x06

// Clock Source - user parameters
#define INTERNALOSC         0   // default
#define PLL_XGYRO_REF       1
#define PLL_YGYRO_REF       2
#define PLL_ZGYRO_REF       3
#define PLL_EXTERNAL32      4   // 32.768 kHz
#define PLL_EXTERNAL19      5   // 19.2 Mhz


#define ITG3205_RA_I_AM     0x68

////////////////////////////////////////PUBLIC STRUCTURES////////////////////////////////////////
typedef struct
{
	Int16S x;
	Int16S y;
	Int16S z;
}GyroscopeRawData;

//-----------------------------------------------------------------------------------------------//
typedef struct
{
	GyroscopeRawData rawData; 
	float roulis;
	float tangage;
	float lacet;
}Gyroscope;
////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////

//fonction init du capteur
Boolean CmpITG3205Init(void);
//Rotation X Y Z
Boolean CmpITG3205GetRotation(Gyroscope *rot);
Boolean CmpITG3205IsCalibrate(void);

#endif /*CMPITG3205_H_*/
