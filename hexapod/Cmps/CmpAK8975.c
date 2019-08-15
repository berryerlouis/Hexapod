/*
 * CmpAK8975.c
 *
 * Created: 03/07/2012 11:49:37
 *  Author: berryer
 */ 
/////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
#include "CmpAK8975.h"

#include "Drv/DrvTwi.h"
#include "Drv/DrvTick.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define NB_SAMPLE_TO_CALIB_AK8975	5
////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
Int8U loop_calibration_ak8975 = 0;
static Int16S mag_calib[3] = {0,0,0};	
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
//fonction init du capteur
Boolean CmpAK8975Init(void)
{
	if(DrvTwiReadReg(AK8975_TWI_ADDRESS, AK8975_RA_WIA) == AK8975_RA_WIA_DEVICE_ID)
	{
		DrvTimerDelay10Us(2000);
		DrvTwiWriteReg(AK8975_TWI_ADDRESS, AK8975_RA_CNTL, AK8975_MODE_SINGLE);
		//Calibration du capteur
		//loop_calibration_ak8975 = NB_SAMPLE_TO_CALIB_AK8975;
		mag_calib[0] = 0;
		mag_calib[1] = 0;
		mag_calib[2] = 0;
		return TRUE;
	}
	else
	{
		return FALSE;		
	}
}	

Boolean CmpAK8975IsCalibrate(void)
{
	if(loop_calibration_ak8975 == 0)
	{
		return TRUE;
	}
	return FALSE;
}

//mageration X Y Z
Boolean CmpAK8975GetMagnetomer(S_Mag_Angle *mag)
{
	Int8U buffer[ 6U ] = {0U, 0U, 0U, 0U, 0U, 0U};
	
	if(DrvTwiReadReg(AK8975_TWI_ADDRESS, AK8975_RA_ST1) == (1 << AK8975_ST1_DRDY_BIT))
	{
		if(DrvTwiReadRegBuf(AK8975_TWI_ADDRESS, AK8975_RA_HXL, buffer, 6) != 6)
		{
			return FALSE;
		}
		else
		{
			mag->x = (Int16S)((((buffer[1]) << 8) | buffer[0])>>1);
			mag->y = (Int16S)((((buffer[3]) << 8) | buffer[2])>>1);
			mag->z = (Int16S)((((buffer[5]) << 8) | buffer[4])>>1);

			if(loop_calibration_ak8975 > 0)
			{
				if( loop_calibration_ak8975 == 1 )
				{
					mag_calib[0] = mag_calib[0] / NB_SAMPLE_TO_CALIB_AK8975;
					mag_calib[1] = mag_calib[1] / NB_SAMPLE_TO_CALIB_AK8975;
					mag_calib[2] = mag_calib[2] / NB_SAMPLE_TO_CALIB_AK8975;
				}
				else
				{
					mag_calib[0] += mag->x;
					mag_calib[1] += mag->y;
					mag_calib[2] += mag->z;
				}
				loop_calibration_ak8975--;
			}
			else
			{
				mag->x  -= mag_calib[0];
				mag->y  -= mag_calib[1];
				mag->z  -= mag_calib[2];
			}				
			DrvTwiWriteReg(AK8975_TWI_ADDRESS, AK8975_RA_CNTL, AK8975_MODE_SINGLE);
			return TRUE;
		}
	}
			
	
}