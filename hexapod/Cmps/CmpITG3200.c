/*
 * CmpITG3200.c
 *
 * Created: 03/07/2012 13:48:49
 *  Author: berryer
 */
 /////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
 #include "CmpITG3200.h"
 #include "Drv/DrvTwi.h"
 #include "Drv/DrvTick.h"

 ////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////

#define NB_SAMPLE_TO_CALIB_ITG3200	50
Int8U loop_calibration_itg3200 = 0;
static Int16S gyro_calib[3] = {0,0,0};
 ////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////


 /////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////

//fonction init du capteur
Boolean CmpITG3200Init(void)
{
	if (DrvTwiReadReg(ITG3200_TWI_ADDRESS,ITG3200_RA_WHO_AM_I) == ITG3200_RA_I_AM)
	{
		DrvTwiWriteReg(ITG3200_TWI_ADDRESS, ITG3200_RA_PWR_MGM, ITG3200_PWR_H_RESET_BIT);
		DrvTimerDelay(2);
		DrvTwiWriteReg(ITG3200_TWI_ADDRESS, ITG3200_RA_DLPF_FS, FS_RANGE_2000 | LPFBW_98HZ );
		DrvTimerDelay(2);
		DrvTwiWriteReg(ITG3200_TWI_ADDRESS, ITG3200_RA_PWR_MGM, PLL_ZGYRO_REF );
		DrvTimerDelay(500);
		//Calibration du capteur
		loop_calibration_itg3200 = NB_SAMPLE_TO_CALIB_ITG3200;
		gyro_calib[0] = 0;
		gyro_calib[1] = 0;
		gyro_calib[2] = 0;
		return TRUE;
	}		
	else
	{
		return FALSE;
	}
	
}

Boolean CmpITG3200IsCalibrate(void)
{
	if(loop_calibration_itg3200 == 0)
	{
		return TRUE;
	}
	return FALSE;
}

//Rotation X Y Z
Boolean CmpITG3200GetRotation(S_Gyr_Angle *rot)
{	
	Int8U buffer[ 6U ] = {0U, 0U, 0U, 0U, 0U, 0U};
		
	if(DrvTwiReadRegBuf(ITG3200_TWI_ADDRESS, ITG3200_RA_GYRO_XOUT_H, buffer, 6) != 6)
	{
		return FALSE;
	}
	else 
	{
		rot->x = (Int16S)((Int16U) buffer[0] << 8) | ((Int16U) buffer[1]);    
		rot->y = (Int16S)((Int16U) buffer[2] << 8) | ((Int16U) buffer[3]);   
		rot->z = (Int16S)((Int16U) buffer[4] << 8) | ((Int16U) buffer[5]);  
	
		if(loop_calibration_itg3200 > 0)
		{
			if( loop_calibration_itg3200 == 1 )
			{
				gyro_calib[0] = gyro_calib[0] / NB_SAMPLE_TO_CALIB_ITG3200;
				gyro_calib[1] = gyro_calib[1] / NB_SAMPLE_TO_CALIB_ITG3200;
				gyro_calib[2] = gyro_calib[2] / NB_SAMPLE_TO_CALIB_ITG3200;
			}
			else
			{
				gyro_calib[0] += rot->x;
				gyro_calib[1] += rot->y;
				gyro_calib[2] += rot->z;
			}
			loop_calibration_itg3200--;
		}
		else
		{
			rot->x  -= gyro_calib[0];
			rot->y  -= gyro_calib[1];
			rot->z  -= gyro_calib[2];
		}
		return TRUE;
	}	
}