/*
 * CmpITG3200.h
 *
 * Created: 03/07/2012 13:49:01
 *  Author: berryer
 */ 


#ifndef CMPITG3200_H_
#define CMPITG3200_H_



#include "Tools/tools_typedefs.h"

////////////////////////////////////////PUBLIC DEFINES/////////////////////////////////////////

#define ITG3200_TWI_ADDRESS				0x68

#define ITG3200_RA_WHO_AM_I				0x00
#define ITG3200_RA_SMPLRT_DIV			0x15
#define ITG3200_RA_DLPF_FS				0x16
#define ITG3200_RA_INT_CFG				0x17
#define ITG3200_RA_INT_STATUS			0x1A
#define ITG3200_RA_TEMP_OUT_H			0x1B
#define ITG3200_RA_TEMP_OUT_L			0x1C
#define ITG3200_RA_GYRO_XOUT_H			0x1D
#define ITG3200_RA_GYRO_XOUT_L			0x1E
#define ITG3200_RA_GYRO_YOUT_H			0x1F
#define ITG3200_RA_GYRO_YOUT_L			0x20
#define ITG3200_RA_GYRO_ZOUT_H			0x21
#define ITG3200_RA_GYRO_ZOUT_L			0x22
#define ITG3200_RA_PWR_MGM				0x3E

#define NO_SMPLRT_DIV					0x00

#define FS_RANGE_2000					0b00011000
#define ITG3200_PWR_H_RESET_BIT			0b10000000


#define LPFBW_256HZ						0b00000000
#define LPFBW_188HZ						0b00000001
#define LPFBW_98HZ						0b00000010
#define LPFBW_42HZ						0b00000011
#define LPFBW_20HZ						0b00000100
#define LPFBW_10HZ						0b00000101
#define LPFBW_5HZ						0b00000110

// Clock Source - user parameters
#define INTERNALOSC         0   // default
#define PLL_XGYRO_REF       1
#define PLL_YGYRO_REF       2
#define PLL_ZGYRO_REF       3
#define PLL_EXTERNAL32      4   // 32.768 kHz
#define PLL_EXTERNAL19      5   // 19.2 Mhz


#define ITG3200_RA_I_AM     0x69

typedef struct 
{
	Int16S x; 
	Int16S y; 
	Int16S z;
}S_Gyr_Angle;

////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////

//fonction init du capteur
Boolean CmpITG3200Init(void);
//Rotation X Y Z
Boolean CmpITG3200GetRotation(S_Gyr_Angle *rot);
Boolean CmpITG3200IsCalibrate(void);


#endif /*CMPITG3200_H_*/
