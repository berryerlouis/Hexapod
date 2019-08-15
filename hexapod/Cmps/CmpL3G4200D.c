/*
 * CmpL3G4200D.c
 *
 * Created: 03/07/2012 13:48:49
 *  Author: berryer
 */
/////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
#include "CmpL3G4200D.h"
 
#include "Drv/DrvTwi.h"
#include "Drv/DrvTick.h"
#include "Drv/DrvEeprom.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////


////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////


////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
static Boolean CmpL3G4200DInit( E_Gyr_Setup setup );
static Boolean CmpL3G4200DReadData( void );
static Int8S CmpL3G4200DGetTemperature(void);
//definition de la structure des fonctions du gyroscope pour le composant l3g4200d
GyroscopeFunctions l3g4200d =
{
	CmpL3G4200DInit,
	CmpL3G4200DReadData,
	CmpL3G4200DGetTemperature
};

//definition de la structure des data gyroscope pour le composant l3g4200d
GyroscopeData l3g4200dData;
/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////

//fonction init du capteur
static Boolean CmpL3G4200DInit(E_Gyr_Setup setup)
{
	Boolean o_success = FALSE;
	Int8U datum = 0U;
	
	DrvTwiReadReg(L3G4200D_ADDRESS, L3G4200D_WHO_AM_I, &datum );
	if(L3G4200D_WHO_I_AM == datum)
	{
		DrvTwiWriteReg(L3G4200D_ADDRESS, L3G4200D_CTRL_REG1, L3G4200D_CTRL_REG1_DR_200HZ	|
															 L3G4200D_CTRL_REG1_BW_10	|
															 L3G4200D_CTRL_REG1_PD	|
															 L3G4200D_CTRL_REG1_ZEN	|
															 L3G4200D_CTRL_REG1_YEN |
															 L3G4200D_CTRL_REG1_XEN	);
		DrvTickDelayUs(200);
		DrvTwiWriteReg(L3G4200D_ADDRESS, L3G4200D_CTRL_REG2, L3G4200D_CTRL_REG2_NO_HI_PASS);
		DrvTickDelayUs(200);
		DrvTwiWriteReg(L3G4200D_ADDRESS, L3G4200D_CTRL_REG3, L3G4200D_CTRL_REG3_NO_INTERRUPT);
		DrvTickDelayUs(200);
		if(setup == GYR_250DPS)
		{
			DrvTwiWriteReg(L3G4200D_ADDRESS, L3G4200D_CTRL_REG4, L3G4200D_CTRL_REG4_FS_250DPS | L3G4200D_CTRL_REG4_BDU_ENABLE);
			l3g4200dData.degreePerSeconds = 0.00875F;
		}
		else if(setup == GYR_500DPS)
		{
			DrvTwiWriteReg(L3G4200D_ADDRESS, L3G4200D_CTRL_REG4, L3G4200D_CTRL_REG4_FS_500DPS | L3G4200D_CTRL_REG4_BDU_ENABLE);
			l3g4200dData.degreePerSeconds = 0.0175F;
		}
		else if(setup == GYR_2000DPS)
		{
			DrvTwiWriteReg(L3G4200D_ADDRESS, L3G4200D_CTRL_REG4, L3G4200D_CTRL_REG4_FS_2000DPS | L3G4200D_CTRL_REG4_BDU_ENABLE);
			l3g4200dData.degreePerSeconds = 0.07F;
		} 
		DrvTickDelayUs(200);
		DrvTwiWriteReg(L3G4200D_ADDRESS, L3G4200D_CTRL_REG5, L3G4200D_CTRL_REG5_ENABLE_LPF2);
		DrvTickDelayUs(200);
		
		//Calibration du capteur
		for( Int8U loopAxis = 0U ; loopAxis < NB_AXIS ; loopAxis++ )
		{
			l3g4200dData.calibrationData.axis.axes[ loopAxis ] = 0;
		}
		l3g4200dData.calibrationData.status = FALSE;
		l3g4200dData.noise.x = 0.0;
		l3g4200dData.noise.y = 0.0;
		l3g4200dData.noise.z = 0.0;	
		o_success = TRUE;
	}
	return o_success;
}

//Get temperature
static Int8S CmpL3G4200DGetTemperature( void )
{
	Int8S temp = 0U;
	DrvTwiReadReg(L3G4200D_ADDRESS, L3G4200D_TEMP_OUT_REG, (Int8U *)&temp);
	return (Int8S)(temp | 0x10);
	
}

//l3g4200dation X Y Z
static Boolean CmpL3G4200DReadData( void )
{
	Boolean oSuccess = FALSE;
	Int8U buffer[ 6U ] = {0U, 0U, 0U, 0U, 0U, 0U};
	if(DrvTwiReadRegBuf(L3G4200D_ADDRESS, L3G4200D_OUT_X_L | L3G4200D_AUTO_INCREMENT, buffer, 6U) == TRUE )
	{
		l3g4200dData.rawData.x = (Int16S)(((Int16U) buffer[ 1U ]) << 8U) | buffer[ 0U ];
		l3g4200dData.rawData.y = (Int16S)(((Int16U) buffer[ 3U ]) << 8U) | buffer[ 2U ];
		l3g4200dData.rawData.z = (Int16S)(((Int16U) buffer[ 5U ]) << 8U) | buffer[ 4U ];
		oSuccess = TRUE;
	}
	return oSuccess;
}



