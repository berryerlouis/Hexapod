/* 
* CmpMPU9150.c
*
* Created: 17/02/2019 22:53:39
* Author: louis
*/
#include "CmpMPU9150.h"

#include "Drv/DrvTwi.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define NB_SAMPLES		20
////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////

SMpu9150Acc accOffset={ 0, 0, 0};
SMpu9150Gyr gyrOffset={ 0, 0, 0};
SMpu9150Acc accOffsetMoy;
SMpu9150Gyr gyrOffsetMoy;
uint8_t accOffsetIndex = 0U;
uint8_t gyrOffsetIndex = 0U;
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
Boolean CmpMPU9150Init(Int8U addr)
{
	Boolean oSuccess = FALSE;
	uint8_t whoAmI = 0x00U;
	uint8_t reg = 0x00U;
	
	//check device
	DrvTwiReadReg( addr, MPU9150_WHOAMI, &whoAmI );
	if(whoAmI == addr - 1)
	{
		oSuccess = TRUE;
	}
	
	//setClockSource
	DrvTwiReadReg( addr, MPU9150_PWR_MGMT_1, &reg );
	reg |= MPU9150_CLOCK_PLL_XGYRO;
	DrvTwiWriteReg(addr, MPU9150_PWR_MGMT_1, reg);
	
    //setFullScaleGyroRange
    DrvTwiReadReg( addr, MPU9150_GYRO_CONFIG, &reg );
	reg |= (MPU9150_GYRO_FS_250 << 3U);
	DrvTwiWriteReg(addr, MPU9150_GYRO_CONFIG, reg);
	
    //setFullScaleAccelRange
    DrvTwiReadReg( addr, MPU9150_ACCEL_CONFIG, &reg );
    reg |= (MPU9150_ACCEL_FS_8 << 3U);
	DrvTwiWriteReg(addr, MPU9150_ACCEL_CONFIG, reg);
	
	//setBandWidthLowPassFilter
	DrvTwiReadReg( addr, MPU9150_CONFIG, &reg );
    reg |= MPU9150_DLPF_BW_20;
	DrvTwiWriteReg(addr, MPU9150_CONFIG, reg);
	
    //setSleepEnabled
    DrvTwiReadReg( addr, MPU9150_PWR_MGMT_1, &reg );
    reg &= !(FALSE << MPU9150_PWR1_SLEEP_BIT);
    DrvTwiWriteReg(addr, MPU9150_PWR_MGMT_1, reg);
		
	return oSuccess;
}


Boolean CmpMPU9150IsInitialized(void)
{	
	Boolean oSuccess = FALSE;
	if((accOffsetIndex == NB_SAMPLES) && (gyrOffsetIndex == NB_SAMPLES))
	{
		oSuccess = TRUE;
	}
	return oSuccess;
}

Boolean CmpMPU9150ReadAcc(Int8U addr, SMpu9150Acc * acc)
{
	Boolean oSuccess = FALSE;
	oSuccess = DrvTwiReadRegBuf(addr,MPU9150_ACCEL_XOUT_H, (uint8_t*) acc, 6U);
	if(oSuccess)
	{
		acc->accX = ((acc->accX>>8U) | (acc->accX<<8U)) - accOffset.accX;
		acc->accY = ((acc->accY>>8U) | (acc->accY<<8U)) - accOffset.accY;
		acc->accZ = ((acc->accZ>>8U) | (acc->accZ<<8U)) - accOffset.accZ + MPU9150_ACCEL_1G_8G;
	}
	if(accOffsetIndex < NB_SAMPLES)
	{
		accOffsetMoy.accX += acc->accX ;
		accOffsetMoy.accY += acc->accY ;
		accOffsetMoy.accZ += acc->accZ ;
		if(accOffsetIndex == NB_SAMPLES - 1U)
		{
			accOffset.accX = accOffsetMoy.accX / NB_SAMPLES;
			accOffset.accY = accOffsetMoy.accY / NB_SAMPLES;
			accOffset.accZ = accOffsetMoy.accZ / NB_SAMPLES;
		}
		accOffsetIndex++;
	}
	
	return oSuccess;
}


Boolean CmpMPU9150ReadGyr(Int8U addr, SMpu9150Gyr * gyr)
{
	Boolean oSuccess = FALSE;
	oSuccess = DrvTwiReadRegBuf(addr,MPU9150_GYRO_XOUT_H, (uint8_t*) gyr, 6U);
	if(oSuccess)
	{
		gyr->gyrX = ((gyr->gyrX>>8U) | (gyr->gyrX<<8U)) - gyrOffset.gyrX;
		gyr->gyrY = ((gyr->gyrY>>8U) | (gyr->gyrY<<8U)) - gyrOffset.gyrY;
		gyr->gyrZ = ((gyr->gyrZ>>8U) | (gyr->gyrZ<<8U)) - gyrOffset.gyrZ;
	}
	if(gyrOffsetIndex < NB_SAMPLES)
	{
		gyrOffsetMoy.gyrX += gyr->gyrX ;
		gyrOffsetMoy.gyrY += gyr->gyrY ;
		gyrOffsetMoy.gyrZ += gyr->gyrZ ;
		if(gyrOffsetIndex == NB_SAMPLES - 1U)
		{
			gyrOffset.gyrX = gyrOffsetMoy.gyrX / NB_SAMPLES;
			gyrOffset.gyrY = gyrOffsetMoy.gyrY / NB_SAMPLES;
			gyrOffset.gyrZ = gyrOffsetMoy.gyrZ / NB_SAMPLES;
		}
		gyrOffsetIndex++;
	}
	return oSuccess;
}

Boolean CmpMPU9150ReadTmp(Int8U addr, SMpu9150Tmp * tmp)
{
	Boolean oSuccess = FALSE;
	oSuccess = DrvTwiReadRegBuf(addr,MPU9150_TEMP_OUT_H, (uint8_t*) tmp, 2U);
	if(oSuccess)
	{
		*tmp = (*tmp>>8U) | (*tmp<<8U);
		*tmp = (*tmp/340.0) + 35U;
	}
	return oSuccess;
}

Boolean CmpMPU9150ReadCmps(Int8U addr, SMpu9150GCmps * cmps)
{
	Boolean oSuccess = FALSE;
	oSuccess = DrvTwiReadRegBuf(addr,MPU9150_CMPS_XOUT_L, (uint8_t*) cmps, 6U);
	if(oSuccess)
	{
		cmps->cmpsX = (cmps->cmpsX>>8U) | (cmps->cmpsX<<8U);
		cmps->cmpsY = (cmps->cmpsY>>8U) | (cmps->cmpsY<<8U);
		cmps->cmpsZ = (cmps->cmpsZ>>8U) | (cmps->cmpsZ<<8U);
	}
	return oSuccess;
}