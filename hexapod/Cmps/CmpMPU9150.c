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
SMpu9150GCmps cmpsOffset={ 0, 0, 0};
SMpu9150Acc accOffsetMoy;
SMpu9150Gyr gyrOffsetMoy;
SMpu9150GCmps cmpsOffsetMoy;
uint8_t accOffsetIndex = 0U;
uint8_t gyrOffsetIndex = 0U;
uint8_t cmpsOffsetIndex = 0U;
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
Boolean CmpMPU9150Init(Int8U addr)
{
	Boolean oSuccess = FALSE;
	uint8_t whoAmI = 0x00U;
	uint8_t reg = 0x00U;
	
	//check device
	DrvTwiReadReg( addr, MPU9150_WHO_AM_I, &whoAmI );
	if(whoAmI == addr - 1)
	{
		oSuccess = TRUE;
	}
	
	//setSleepEnabled
	DrvTwiReadReg( addr, MPU9150_PWR_MGMT_1, &reg );
	reg &= !(FALSE << MPU9150_PWR1_SLEEP_BIT);
	DrvTwiWriteReg(addr, MPU9150_PWR_MGMT_1, reg);
	
	DrvTwiWriteReg(MPU9150_CMPS_ADDRESS, MPU9150_CMPS_CONTROL, 0x00); //PowerDownMode
	DrvTwiWriteReg(MPU9150_CMPS_ADDRESS, MPU9150_CMPS_CONTROL, 0x0F); //SelfTest
	DrvTwiWriteReg(MPU9150_CMPS_ADDRESS, MPU9150_CMPS_CONTROL, 0x00); //PowerDownMode
	
	DrvTwiWriteReg( addr, MPU9150_I2C_MST_CTRL, 0x40); //Wait for Data at Slave0
	DrvTwiWriteReg( addr, MPU9150_I2C_SLV0_ADDR, 0x8C); //Set i2c address at slave0 at 0x0C
	DrvTwiWriteReg( addr, MPU9150_I2C_SLV0_REG, 0x02); //Set where reading at slave 0 starts
	DrvTwiWriteReg( addr, MPU9150_I2C_SLV0_CTRL, 0x88); //set offset at start reading and enable
	DrvTwiWriteReg( addr, MPU9150_I2C_SLV1_ADDR, 0x0C); //set i2c address at slv1 at 0x0C
	DrvTwiWriteReg( addr, MPU9150_I2C_SLV1_REG, 0x0A); //Set where reading at slave 1 starts
	DrvTwiWriteReg( addr, MPU9150_I2C_SLV1_CTRL, 0x81); //Enable at set length to 1
	DrvTwiWriteReg( addr, MPU9150_I2C_SLV1_DO, 0x01); //overvride register
	DrvTwiWriteReg( addr, MPU9150_I2C_MST_DELAY_CTRL, 0x03); //set delay rate
	
	DrvTwiWriteReg( addr, MPU9150_I2C_SLV4_CTRL, 0x04); //set i2c slv4 delay
	DrvTwiWriteReg( addr, MPU9150_I2C_SLV1_DO, 0x00); //override register
	DrvTwiWriteReg( addr, MPU9150_USER_CTRL, 0x00); //clear usr setting
	DrvTwiWriteReg( addr, MPU9150_I2C_SLV1_DO, 0x01); //override register
	DrvTwiWriteReg( addr, MPU9150_USER_CTRL, 0x20); //enable master i2c mode
	DrvTwiWriteReg( addr, MPU9150_I2C_SLV4_CTRL, 0x13); //disable slv4
	
	
	//setClockSource
	DrvTwiReadReg( addr, MPU9150_PWR_MGMT_1, &reg );
	reg |= MPU9150_CLOCK_PLL_XGYRO;
	DrvTwiWriteReg(addr, MPU9150_PWR_MGMT_1, reg);
	
    //setFullScaleGyroRange
    DrvTwiReadReg( addr, MPU9150_GYRO_CONFIG, &reg );
	reg |= (MPU9150_GYRO_FS_2000 << 3U);
	DrvTwiWriteReg(addr, MPU9150_GYRO_CONFIG, reg);
	
    //setFullScaleAccelRange
    DrvTwiReadReg( addr, MPU9150_ACCEL_CONFIG, &reg );
    reg |= (MPU9150_ACCEL_FS_2 << 3U);
	DrvTwiWriteReg(addr, MPU9150_ACCEL_CONFIG, reg);
	
	//setBandWidthLowPassFilter
	DrvTwiReadReg( addr, MPU9150_CONFIG, &reg );
    reg |= MPU9150_DLPF_BW_10;
	DrvTwiWriteReg(addr, MPU9150_CONFIG, reg);
	
		
	return oSuccess;
}

Boolean CmpMPU9150IsCalibrated(void)
{	
	Boolean oSuccess = FALSE;
	if((accOffsetIndex == NB_SAMPLES) && (gyrOffsetIndex == NB_SAMPLES) && (cmpsOffsetIndex == NB_SAMPLES))
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
		acc->accX = (int16_t)((acc->accX << 8) | ((acc->accX >> 8) & 0xFF)) - accOffset.accX;
		acc->accY = (int16_t)((acc->accY << 8) | ((acc->accY >> 8) & 0xFF)) - accOffset.accY;
		acc->accZ = (int16_t)((acc->accZ << 8) | ((acc->accZ >> 8) & 0xFF)) - accOffset.accZ + MPU9150_ACCEL_1G_8G;
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
		gyr->gyrX = (int16_t)((gyr->gyrX << 8) | ((gyr->gyrX >> 8) & 0xFF)) - gyrOffset.gyrX;
		gyr->gyrY = (int16_t)((gyr->gyrY << 8) | ((gyr->gyrY >> 8) & 0xFF)) - gyrOffset.gyrY;
		gyr->gyrZ = (int16_t)((gyr->gyrZ << 8) | ((gyr->gyrZ >> 8) & 0xFF)) - gyrOffset.gyrZ;
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
		*tmp =  (int16_t)((*tmp << 8) | ((*tmp >> 8) & 0xFF));
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
		cmps->cmpsX -= cmpsOffset.cmpsX;
		cmps->cmpsY -= cmpsOffset.cmpsY;
		cmps->cmpsZ -= cmpsOffset.cmpsZ;
	}
	if(cmpsOffsetIndex < NB_SAMPLES)
	{
		cmpsOffsetMoy.cmpsX += cmps->cmpsX ;
		cmpsOffsetMoy.cmpsY += cmps->cmpsY ;
		cmpsOffsetMoy.cmpsZ += cmps->cmpsZ ;
		if(accOffsetIndex == NB_SAMPLES - 1U)
		{
			cmpsOffset.cmpsX = cmpsOffsetMoy.cmpsX / NB_SAMPLES;
			cmpsOffset.cmpsY = cmpsOffsetMoy.cmpsY / NB_SAMPLES;
			cmpsOffset.cmpsZ = cmpsOffsetMoy.cmpsZ / NB_SAMPLES;
		}
		cmpsOffsetIndex++;
	}
	
	return oSuccess;	
}

	