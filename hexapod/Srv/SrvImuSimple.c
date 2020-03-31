////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Cmps/CmpMPU9150.h"

#include "Drv/DrvTick.h"

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
	imu.accthreshold = 5U;
	return CmpMPU9150Init(MPU9150_ADDRESS);
}

static volatile float roll , pitch, yaw;
void SrvImuSimpleUpdate (void)
{
	if(	imu.enable == TRUE )
	{
		
		// ********************* Read Data *************************	
		CmpMPU9150ReadAcc(MPU9150_ADDRESS, &imu.acc);
		CmpMPU9150ReadGyr(MPU9150_ADDRESS, &imu.gyr);
		CmpMPU9150ReadTmp(MPU9150_ADDRESS, &imu.tmp);
		CmpMPU9150ReadCmps(MPU9150_ADDRESS, &imu.cmps);
				
		//check if hexapod is shoving
		if(CmpMPU9150IsCalibrated())
		{
			//compute axis
			float accelX = imu.acc.accX;
			float accelY = imu.acc.accY;
			float accelZ = imu.acc.accZ;
			//float gyroX = (imu.gyr.gyrX * 180.0) / M_PI;
			//float gyroY = (imu.gyr.gyrY * 180.0) / M_PI;
			//float gyroZ = (imu.gyr.gyrZ * 180.0) / M_PI;
			//float magX = imu.cmps.cmpsX;
			//float magY = imu.cmps.cmpsY;
			//float magZ = imu.cmps.cmpsZ;
	
			//Roll & Pitch from acc
			roll = atan2(-accelX ,( sqrt((accelY * accelY) + (accelZ * accelZ))));
			pitch = atan2 (accelY ,( sqrt ((accelX * accelX) + (accelZ * accelZ))));

			// yaw from mag
			//float Yh = (magY * cos(roll)) - (magZ * sin(roll));
			//float Xh = (magX * cos(pitch))+(magY * sin(roll)*sin(pitch)) + (magZ * cos(roll) * sin(pitch));
			//yaw = gyroZ;

			//filter data
			imu.roll = (Int16S)ToDeg(roll) * -1;
			imu.pitch = (Int16S)ToDeg(pitch);
			imu.yaw = 0;
			
			
			if(
				(abs(imu.roll) > imu.accthreshold) ||
				(abs(imu.pitch) > imu.accthreshold) 
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
// set threshold
Boolean SrvImuSimpleSetThreshold( uint8_t accthreshold )
{
	imu.accthreshold = accthreshold;
	return TRUE;
}
// get threshold
uint8_t SrvImuSimpleGetThreshold( void )
{
	return imu.accthreshold;
}
// get roll
Int16S SrvImuSimpleGetRoll( void )
{
	return imu.roll;
}
// get pitch
Int16S SrvImuSimpleGetPitch( void )
{
	return imu.pitch;
}
// get yaw
Int16S SrvImuSimpleGetYaw( void )
{
	return imu.yaw;
}
//Get the sensor pointer
SImuSimple* SrvImuSimpleGetSensor (void)
{
	return &imu;
}