/*
 * CmpHMC5883.c
 *
 * Created: 03/07/2012 13:48:49
 *  Author: berryer
 */
/////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
#include "CmpHMC5883.h"

#include "Drv/DrvTwi.h"
#include "Drv/DrvTick.h"

////////////////////////////////////////PRIVATE DEFINES////////////////////////////////////////////


////////////////////////////////////////PRIVATE VARIABLES//////////////////////////////////////////

Int32S overflowGain[ 3U ] = { 0, 0, 0 };
float selectedScale = 0.0F;
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
static Boolean CmpHMC5883Init(E_Mag_Setup setup);
static Boolean CmpHMC5883GetReadData( void );
static Boolean CmpHMC5883GetBias(Int8U bias) ;
static Int8U CmpHMC5883SetScale(E_Mag_Setup gauss);


//definition de la structure des fonctions de accelerometer pour le composant HMC5886
MagnetometerFunctions hmc5883 =
{
	CmpHMC5883Init,
	CmpHMC5883GetReadData
};

//definition de la structure des data accelerometer pour le composant HMC5886
MagnetometerData hmc5883Data;
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

//fonction init du capteur
Boolean CmpHMC5883Init(E_Mag_Setup gauss)
{
	Boolean oSuccess = FALSE;
	DrvTwiWriteReg(HMC5883_ADDRESS,HMC5883_CONFIG_B,HMC5883_1_9GA << 5U);
	DrvTwiWriteReg(HMC5883_ADDRESS,HMC5883_CONFIG_B,HMC5883_SINGLE);
	DrvTickDelayMs(1);
	
	//set default gain
	for( Int8U loopAxis = 0U ; loopAxis < NB_AXIS ; loopAxis++ )
	{
		hmc5883Data.gainData.axes[ loopAxis ] = 1.0F;
	}
	
	CmpHMC5883GetReadData();
	oSuccess = TRUE;
	if(CmpHMC5883GetBias(HMC5883_POSITIVE_BIAS) == FALSE)
	{
		oSuccess = FALSE;
	}
	if(CmpHMC5883GetBias(HMC5883_NEGATIVE_BIAS) == FALSE)
	{
		oSuccess = FALSE;
	}
	
	if (oSuccess == TRUE)
	{
		//set computed gain
		for( Int8U loopAxis = 0U ; loopAxis < NB_AXIS ; loopAxis++ )
		{
			hmc5883Data.gainData.axes[ loopAxis ] = 820.0*(+1.16)*2.0*10.0/overflowGain[ loopAxis ];
		}
	}
	
	
	oSuccess = CmpHMC5883SetScale(gauss);
	DrvTwiWriteReg(HMC5883_ADDRESS,HMC5883_CONFIG_A, (HMC5883_8_SAMPLE << 5U ) + (HMC5883_15HZ << 2U) + (HMC5883_NORMAL_BIAS << 0U ) );
	DrvTwiWriteReg(HMC5883_ADDRESS,HMC5883_MODE,HMC5883_CONTINUOUS);
	DrvTickDelayMs(5);
	oSuccess = TRUE;
	return oSuccess;
}

static Boolean CmpHMC5883GetReadData( void )
{
	Boolean oSuccess = FALSE;
	Int8U buffer[ 6U ] = {0U, 0U, 0U, 0U, 0U, 0U};
	if(DrvTwiReadRegBuf(HMC5883_ADDRESS, HMC5883_X_MSB, buffer, 6U) == TRUE )
	{
		hmc5883Data.rawData.x = (Int16S)((Int16U)(buffer[ 0U ] << 8U) | (Int16U)(buffer[ 1U ]));
		hmc5883Data.rawData.y = (Int16S)((Int16U)(buffer[ 4U ] << 8U) | (Int16U)(buffer[ 5U ]));
		hmc5883Data.rawData.z = (Int16S)((Int16U)(buffer[ 2U ] << 8U) | (Int16U)(buffer[ 3U ]));
		oSuccess = TRUE;
	}
	return oSuccess;
}


static Boolean CmpHMC5883GetBias(Int8U bias) 
{
	Int16U absoluteGain;

	DrvTwiWriteReg(HMC5883_ADDRESS,HMC5883_CONFIG_A, (HMC5883_1_SAMPLE << 5U ) + (HMC5883_15HZ << 2U) + (bias << 0U ) );
	for (Int8U i=0; i<10; i++) 
	{
		DrvTwiWriteReg(HMC5883_ADDRESS,HMC5883_CONFIG_B,HMC5883_SINGLE);
		DrvTickDelayMs(1);
		CmpHMC5883GetReadData();
	
		for( Int8U loopAxis = 0U ; loopAxis < NB_AXIS ; loopAxis++ )
		{
			absoluteGain = abs(hmc5883Data.rawData.axes[ loopAxis ]);
			overflowGain[ loopAxis ] += absoluteGain;
			if ((Int16U)(1<<12) < absoluteGain)
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

static Boolean CmpHMC5883SetScale(E_Mag_Setup gauss)
{
	Int8U regValue = 0x00U;
	if(gauss == MAG_0_88GA)
	{
		regValue = HMC5883_0_88GA;
		selectedScale = 0.73;
	}
	else if(gauss == MAG_1_3GA)
	{
		regValue = HMC5883_1_3GA;
		selectedScale = 0.92;
	}
	else if(gauss == MAG_1_9GA)
	{
		regValue = HMC5883_1_9GA;
		selectedScale = 1.22;
	}
	else if(gauss == MAG_2_5GA)
	{
		regValue = HMC5883_2_5GA;
		selectedScale = 1.52;
	}
	else if(gauss == MAG_4_0GA)
	{
		regValue = HMC5883_4_0GA;
		selectedScale = 2.27;
	}
	else if(gauss == MAG_4_7GA)
	{
		regValue = HMC5883_4_7GA;
		selectedScale = 2.56;
	}
	else if(gauss == HMC5883_5_6GA)
	{
		regValue = MAG_5_6GA;
		selectedScale = 3.03;
	}
	else if(gauss == MAG_8_1GA)
	{
		regValue = HMC5883_8_1GA;
		selectedScale = 4.35;
	}
	else
	{
		return FALSE;
	}
	
	return DrvTwiWriteReg(HMC5883_ADDRESS,HMC5883_CONFIG_B,regValue << 5U);
}

