/* 
* CmpSSD1306.h
*
* Created: 17/02/2019 22:53:39
* Author: louis
*/


#ifndef __CMPSMPU9150_H__
#define __CMPSMPU9150_H__


#include "Conf/ConfHard.h"

////////////////////////////////////////PUBLIC DEFINES/////////////////////////////////////////

#define MPU9150_ADDRESS						0x69U ///< I2C address of the MPU9150 module


#define MPU9150_CONFIG						26U
#define MPU9150_GYRO_CONFIG					27U
#define MPU9150_ACCEL_CONFIG				28U
#define MPU9150_ACCEL_XOUT_H				59U
#define MPU9150_ACCEL_XOUT_L				60U
#define MPU9150_ACCEL_YOUT_H				61U
#define MPU9150_ACCEL_YOUT_L				62U
#define MPU9150_ACCEL_ZOUT_H				63U
#define MPU9150_ACCEL_ZOUT_L				64U
#define MPU9150_TEMP_OUT_H					65U
#define MPU9150_TEMP_OUT_L					66U
#define MPU9150_GYRO_XOUT_H					67U
#define MPU9150_GYRO_XOUT_L					68U
#define MPU9150_GYRO_YOUT_H					69U
#define MPU9150_GYRO_YOUT_L					70U
#define MPU9150_GYRO_ZOUT_H					71U
#define MPU9150_GYRO_ZOUT_L					72U
#define MPU9150_CMPS_XOUT_L					74U
#define MPU9150_CMPS_XOUT_H					75U
#define MPU9150_CMPS_YOUT_L					76U
#define MPU9150_CMPS_YOUT_H					77U
#define MPU9150_CMPS_ZOUT_L					78U
#define MPU9150_CMPS_ZOUT_H					79U
#define MPU9150_USER_CTRL					106U
#define MPU9150_PWR_MGMT_1					107U
#define MPU9150_PWR_MGMT_2					108U
#define MPU9150_WHOAMI						117U

#define MPU9150_CLOCK_PLL_XGYRO				0x01

#define MPU9150_PWR1_CLKSEL_BIT				2
#define MPU9150_PWR1_SLEEP_BIT				6
	
#define MPU9150_GYRO_FS_250					0x00
#define MPU9150_GYRO_FS_500					0x01
#define MPU9150_GYRO_FS_1000				0x02
#define MPU9150_GYRO_FS_2000				0x03

#define MPU9150_ACCEL_FS_2					0x00
#define MPU9150_ACCEL_FS_4					0x01
#define MPU9150_ACCEL_FS_8					0x02
#define MPU9150_ACCEL_FS_16					0x03

#define MPU9150_DLPF_BW_256					0x00
#define MPU9150_DLPF_BW_188					0x01
#define MPU9150_DLPF_BW_98					0x02
#define MPU9150_DLPF_BW_42					0x03
#define MPU9150_DLPF_BW_20					0x04
#define MPU9150_DLPF_BW_10					0x05
#define MPU9150_DLPF_BW_5					0x06

#define MPU9150_ACCEL_1G_2G					0x3FFF
#define MPU9150_ACCEL_1G_4G					0x1FFF
#define MPU9150_ACCEL_1G_8G					0x0FFF
#define MPU9150_ACCEL_1G_16G				0x07FF

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef struct
{
	int16_t accX;
	int16_t accY;
	int16_t accZ;
}SMpu9150Acc;

typedef struct
{
	int16_t gyrX;
	int16_t gyrY;
	int16_t gyrZ;
}SMpu9150Gyr;

typedef struct
{
	int16_t cmpsX;
	int16_t cmpsY;
	int16_t cmpsZ;
}SMpu9150GCmps;


typedef int16_t SMpu9150Tmp;


////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////

Boolean CmpMPU9150Init(Int8U addr);

Boolean CmpMPU9150IsCalibrated(void);

Boolean CmpMPU9150ReadAcc(Int8U addr, SMpu9150Acc * acc);

Boolean CmpMPU9150ReadGyr(Int8U addr, SMpu9150Gyr * gyr);

Boolean CmpMPU9150ReadTmp(Int8U addr, SMpu9150Tmp * tmp);

Boolean CmpMPU9150ReadCmps(Int8U addr, SMpu9150GCmps * cmps);
#endif //__CMPSMPU9150_H__