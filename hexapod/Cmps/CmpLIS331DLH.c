/*
 * CmpLIS331DLH.c
 *
 * Created: 11/02/2014 12:00:01
 *  Author: berryer
 */ 

/////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
#include "CmpLIS331DLH.h"

#include "Drv/DrvTwi.h"
#include "Drv/DrvTick.h"
#include "Drv/DrvEeprom.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

//fonction init du capteur
static Boolean CmpLIS331DLHInit(E_Acc_Setup setup);

//Acceleration X Y Z
static Boolean CmpLIS331DLHReadData( void );
////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////


//definition de la structure des fonctions de accelerometer pour le composant LIS331DLH
AccelerometerFunctions lis331dlh =
{
	CmpLIS331DLHInit,
	CmpLIS331DLHReadData
};

//definition de la structure des data accelerometer pour le composant LIS331DLH
AccelerometerData lis331dlhData;
 /////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////

//fonction init du capteur
static Boolean CmpLIS331DLHInit(E_Acc_Setup setup)
{
	Boolean oSuccess = FALSE;
	Int8U datum = 0U;

	DrvTwiReadReg(LIS331DLH_ADDRESS, LIS331DLH_WHO_AM_I, &datum );
	if(LIS331DLH_WHO_I_AM == datum)
	{
		DrvTickDelayUs(200);
		//400Hz
		//Normal Mode
		//Enable XYZ
		DrvTwiWriteReg(LIS331DLH_ADDRESS,LIS331DLH_CTRL_REG1,	LIS331DLH_CTRL_REG1_PM0 |
																	LIS331DLH_CTRL_REG1_DR1 |
																	LIS331DLH_CTRL_REG1_ZEN |
																	LIS331DLH_CTRL_REG1_YEN |
																	LIS331DLH_CTRL_REG1_XEN 
																	);
		DrvTickDelayUs(200);
		if((E_Acc_Setup)setup == ACC_1G_TO_2G)
		{
			DrvTwiWriteReg(LIS331DLH_ADDRESS,LIS331DLH_CTRL_REG4,	LIS331DLH_CTRL_REG4_BDU |
			LIS331DLH_CTRL_REG4_FS_1G_2G |
			LIS331DLH_CTRL_REG4_BLE);
			lis331dlhData.gravity = LIS331DLH_ACC_1G_2G;
		}
		else if((E_Acc_Setup)setup == ACC_1G_TO_4G)
		{
			DrvTwiWriteReg(LIS331DLH_ADDRESS,LIS331DLH_CTRL_REG4,	LIS331DLH_CTRL_REG4_BDU |
			LIS331DLH_CTRL_REG4_FS_1G_4G |
			LIS331DLH_CTRL_REG4_BLE);
			lis331dlhData.gravity = LIS331DLH_ACC_1G_4G;
		}
		else if((E_Acc_Setup)setup == ACC_1G_TO_8G)
		{
			DrvTwiWriteReg(LIS331DLH_ADDRESS,LIS331DLH_CTRL_REG4,	LIS331DLH_CTRL_REG4_BDU |
			LIS331DLH_CTRL_REG4_FS_1G_8G |
			LIS331DLH_CTRL_REG4_BLE);
			lis331dlhData.gravity = LIS331DLH_ACC_1G_8G;
		}
				
		oSuccess = TRUE;
	}
	return oSuccess;
}

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////


//Acceleration X Y Z
static Boolean CmpLIS331DLHReadData( void )
{
	Boolean oSuccess = FALSE;
	Int8U buffer[ 6U ] = {0U, 0U, 0U, 0U, 0U, 0U};
	if( DrvTwiReadRegBuf( LIS331DLH_ADDRESS, LIS331DLH_OUT_X_L | 0x80, buffer, 6U ) == TRUE )
	{
		lis331dlhData.rawData.x = (Int16S)((Int16U) buffer[0U] << 8U) | ((Int16U) buffer[1U]);
		lis331dlhData.rawData.y = (Int16S)((Int16U) buffer[2U] << 8U) | ((Int16U) buffer[3U]);
		lis331dlhData.rawData.z = (Int16S)((Int16U) buffer[4U] << 8U) | ((Int16U) buffer[5U]);
		oSuccess = TRUE;
	}
	return oSuccess;
}