////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Cmps/CmpMPU9150.h"

#include "SrvImuSimple.h"
#include "SrvFeeling.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
static SImuSimple imu;
////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////


//Fonction d'initialisation
Boolean SrvImuSimpleInit ( void ) 
{	
	imu.enable = TRUE;
	imu.accthreshold = 50U;
	return CmpMPU9150Init(MPU9150_ADDRESS);
}

//Fonction de dispatching d'evenements
void SrvImuSimpleUpdate (void)
{
	if(	imu.enable == TRUE )
	{
		CmpMPU9150ReadAcc(MPU9150_ADDRESS, &imu.acc);
		CmpMPU9150ReadGyr(MPU9150_ADDRESS, &imu.gyr);
		CmpMPU9150ReadTmp(MPU9150_ADDRESS, &imu.tmp);
	
		//check if hexapod is shoving
		if(CmpMPU9150IsCalibrated())
		{
			if(
			(abs(imu.acc.accX) > imu.accthreshold) ||
			(abs(imu.acc.accY) > imu.accthreshold) 
			)
			{
				SrvFeelingSetFeeling(FEELING_STRESS);
			}
		}
	}
}

// activate the sensor
Boolean SrvImuSimpleActivate( Boolean enable)
{
	imu.enable = enable;
	return TRUE;
}

//Get the sensor pointer
SImuSimple* SrvImuSimpleGetSensor (void)
{
	return &imu;
}