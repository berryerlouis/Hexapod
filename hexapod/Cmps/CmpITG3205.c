/*
 * CmpITG3205.c
 *
 * Created: 03/07/2012 13:48:49
 *  Author: berryer
 */
/////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
#include "CmpITG3205.h"
 
#include "Drv/DrvTwi.h"
#include "Drv/DrvTick.h"
#include "Drv/DrvEeprom.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
//samples to calib
#define NB_SAMPLE_TO_CALIB_ITG3205	100
//limit for glitch
#define GYRO_GLITCH_LIMIT	800
//value for smoothing
#define GYRO_SMOOTHING_X	20
#define GYRO_SMOOTHING_Y	20
#define GYRO_SMOOTHING_Z	3
////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
Int16S previous_reading[ 3U ] = { 0U, 0U, 0U };
Int16S gyro_smooth_value[ 3U ] = { 0U, 0U, 0U };
Int8U loop_calibration_itg3205 = 0U;
Int16S gyro_calib_itg3205[ 3U ] = { 0, 0, 0 };
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////




//fonction init du capteur
Boolean CmpITG3205Init(void)
{
	Boolean conf = FALSE;
	Boolean o_success = FALSE;
	Int8U datum = 0U;
	
	DrvTwiReadReg(ITG3205_TWI_ADDRESS, ITG3205_RA_WHO_AM_I, &datum );
	if(ITG3205_RA_I_AM == datum)
	{
		DrvTwiWriteReg(ITG3205_TWI_ADDRESS, ITG3205_RA_PWR_MGM, ITG3205_PWR_H_RESET_BIT);
		DrvTickDelayUs(200);
		DrvTwiWriteReg(ITG3205_TWI_ADDRESS, ITG3205_RA_SMPLRT_DIV, NO_SMPLRT_DIV);
		DrvTickDelayUs(200);
		DrvTwiWriteReg(ITG3205_TWI_ADDRESS, ITG3205_RA_DLPF_FS, FS_RANGE_2000 | LPFBW_256HZ );
		DrvTickDelayUs(200);
		DrvTwiWriteReg(ITG3205_TWI_ADDRESS, ITG3205_RA_PWR_MGM, PLL_ZGYRO_REF );
		DrvTickDelayUs(200);
		
		//Calibration du capteur
		//si l'eeprom est configué
		conf = DrvEepromIsConfigured();
		if(conf == FALSE)
		{
			loop_calibration_itg3205 = NB_SAMPLE_TO_CALIB_ITG3205;
			gyro_calib_itg3205[ 0U ] = 0;
			gyro_calib_itg3205[ 1U ] = 0;
			gyro_calib_itg3205[ 2U ] = 0;
		}
		else
		{
			loop_calibration_itg3205 = 0U;
			DrvEepromReadGyro(gyro_calib_itg3205);
		}
		o_success = TRUE;
	}	
	return o_success;
}
Boolean CmpITG3205IsCalibrate(void)
{
	if(loop_calibration_itg3205 == 0)
	{
		DrvEepromWriteGyro(gyro_calib_itg3205);
		return TRUE;
	}
	return FALSE;
}

//Rotation X Y Z
Boolean CmpITG3205GetRotation(Gyroscope *rot)
{
	Int8U buffer[ 6U ] = { 0, 0, 0, 0, 0, 0 };

	if(DrvTwiReadRegBuf(ITG3205_TWI_ADDRESS, ITG3205_RA_GYRO_XOUT_H, buffer, 6U) != TRUE )
	{
		return FALSE;
	}
	else
	{
		//read gyro value
		rot->rawData.x = (Int16S)((Int16U) buffer[0U] << 8U) | ((Int16U) buffer[1U]);
		rot->rawData.y = (Int16S)((Int16U) buffer[2U] << 8U) | ((Int16U) buffer[3U]);
		rot->rawData.z = (Int16S)((Int16U) buffer[4U] << 8U) | ((Int16U) buffer[5U]);
		//anti gyro glitch
		//rot->x = SetLimits( rot->x, previous_reading[ 0 ] - GYRO_GLITCH_LIMIT ,previous_reading[ 0 ] + GYRO_GLITCH_LIMIT );
		//rot->y = SetLimits( rot->y, previous_reading[ 1 ] - GYRO_GLITCH_LIMIT ,previous_reading[ 1 ] + GYRO_GLITCH_LIMIT );
		//rot->z = SetLimits( rot->z, previous_reading[ 2 ] - GYRO_GLITCH_LIMIT ,previous_reading[ 2 ] + GYRO_GLITCH_LIMIT );
		
		if(loop_calibration_itg3205 > 0U)
		{
			if( loop_calibration_itg3205 == 1U )
			{
				gyro_calib_itg3205[0U] = gyro_calib_itg3205[0U] / (NB_SAMPLE_TO_CALIB_ITG3205 - 1);
				gyro_calib_itg3205[1U] = gyro_calib_itg3205[1U] / (NB_SAMPLE_TO_CALIB_ITG3205 - 1);
				gyro_calib_itg3205[2U] = gyro_calib_itg3205[2U] / (NB_SAMPLE_TO_CALIB_ITG3205 - 1);
			}
			else
			{
				gyro_calib_itg3205[0U] += rot->rawData.x;
				gyro_calib_itg3205[1U] += rot->rawData.y;
				gyro_calib_itg3205[2U] += rot->rawData.z;
			}
			loop_calibration_itg3205--;
		}
		else
		{
			//read data - zero offset
			rot->rawData.x  -= gyro_calib_itg3205[0U];
			rot->rawData.y  -= gyro_calib_itg3205[1U];
			rot->rawData.z  -= gyro_calib_itg3205[2U];
			
			//smooth gyro value
			/*rot->x = (Int16S) ( ( (Int32S)((Int32S)gyro_smooth_value[0] * (GYRO_SMOOTHING_X - 1) ) + rot->x + 1 ) / GYRO_SMOOTHING_X);
			gyro_smooth_value[0] = rot->x;
			
			rot->y = (Int16S) ( ( (Int32S)((Int32S)gyro_smooth_value[1] * (GYRO_SMOOTHING_Y - 1) ) + rot->y + 1 ) / GYRO_SMOOTHING_Y);
			gyro_smooth_value[1] = rot->y;
			
			rot->z = (Int16S) ( ( (Int32S)((Int32S)gyro_smooth_value[2] * (GYRO_SMOOTHING_Z - 1) ) + rot->z + 1 ) / GYRO_SMOOTHING_Z);
			gyro_smooth_value[2] = rot->z;*/
		}
		return TRUE;
	}
}
