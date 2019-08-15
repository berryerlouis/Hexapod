/*
 * CmpBMA150.c
 *
 * Created: 03/07/2012 11:49:37
 *  Author: berryer
 */ 
/////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
#include "CmpBMA150.h"
#include "Drv/DrvTwi.h"
#include "Drv/DrvTick.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define NB_SAMPLE_TO_CALIB_BMA150	5
////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
Int8U loop_calibration_bma150 = 0;
static Int16S accel_calib[3] = {0,0,0};
	
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
//fonction init du capteur
Boolean CmpBMA150Init(void)
{
	if(DrvTwiReadReg(BMA150_TWI_ADDRESS, BMA150_RA_CHIP_ID) == BMA150_CHIP_ID)
	{
		DrvTwiWriteReg(BMA150_TWI_ADDRESS, BMA150_RA_RANGE_BWIDTH, BMA150_RANGE_4G & BMA150_RANGE_MASK);
		DrvTimerDelay(20);
		DrvTwiWriteReg(BMA150_TWI_ADDRESS, BMA150_RA_RANGE_BWIDTH, BMA150_BW_1500HZ & BMA150_BANDWIDTH_MASK);
		DrvTimerDelay(5000);
		//Calibration du capteur
		//loop_calibration_bma150 = NB_SAMPLE_TO_CALIB_BMA150;
		accel_calib[0] = 0;
		accel_calib[1] = 0;
		accel_calib[2] = 0;
		return TRUE;
	}
	else
	{
		return FALSE;		
	}
}	

Boolean CmpBMA150IsCalibrate(void)
{
	if(loop_calibration_bma150 == 0)
	{
		return TRUE;
	}
	return FALSE;
}	

//Acceleration X Y Z
Boolean CmpBMA150GetAcceleration(S_Acc_Angle *acc)
{
	Int8U buffer[ 6U ] = {0U, 0U, 0U, 0U, 0U, 0U};
	if(DrvTwiReadRegBuf(BMA150_TWI_ADDRESS, BMA150_RA_X_AXIS_LSB, buffer, 6) != 6)
	{
		return FALSE;
	}	
	else
	{	
		acc->x = (Int16S)(((Int16U)(buffer[1]) << 8) | buffer[0]) >> 6;
		acc->y = (Int16S)(((Int16U)(buffer[3]) << 8) | buffer[2]) >> 6;
		acc->z = (Int16S)(((Int16U)(buffer[5]) << 8) | buffer[4]) >> 6;

		if(loop_calibration_bma150 > 0)
		{
			if( loop_calibration_bma150 == 1 )
			{
				accel_calib[0] = accel_calib[0] / NB_SAMPLE_TO_CALIB_BMA150;
				accel_calib[1] = accel_calib[1] / NB_SAMPLE_TO_CALIB_BMA150;
				accel_calib[2] = accel_calib[2] / NB_SAMPLE_TO_CALIB_BMA150;
				accel_calib[2] -= BMA150_1G;
			}
			else
			{
				accel_calib[0] += acc->x;
				accel_calib[1] += acc->y;
				accel_calib[2] += acc->z;
			}
			loop_calibration_bma150--;
		}
		else
		{
			acc->x  -= accel_calib[0];
			acc->y  -= accel_calib[1];
			acc->z  -= accel_calib[2];
		}
		return TRUE;
	}	
}