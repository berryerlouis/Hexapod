////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Cmps/CmpMPU9150.h"

#include "Drv/DrvTick.h"

#include "SrvImuSimple.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
static SImuSimple imu;
static volatile float roll;
static volatile float pitch;
static volatile float yaw;
Int32U prevMillisImuUpdate = 0;
////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////

//Fonction d'initialisation
Boolean SrvImuSimpleInit ( void ) 
{	
	imu.enable = TRUE;
	imu.accthreshold = IMU_THRESHOLD_ANGLE;
	prevMillisImuUpdate = 0;
	roll = 0;
	pitch = 0;
	yaw = 0;
	imu.cbReachThreshold = NULL;
	return CmpMPU9150Init(MPU9150_ADDRESS);
}

void SrvImuSimpleUpdate (void)
{
	if ((DrvTickGetTimeMs() - prevMillisImuUpdate) > 250U)
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
				float gyroX = (imu.gyr.gyrX * 180.0) / M_PI;
				float gyroY = (imu.gyr.gyrY * 180.0) / M_PI;
				float gyroZ = (imu.gyr.gyrZ * 180.0) / M_PI;
				float magX = imu.cmps.cmpsX;
				float magY = imu.cmps.cmpsY;
				float magZ = imu.cmps.cmpsZ;
	
				//Roll & Pitch from acc
				roll = atan2(-accelX ,( sqrt((accelY * accelY) + (accelZ * accelZ))));
				pitch = atan2 (accelY ,( sqrt ((accelX * accelX) + (accelZ * accelZ))));

				// yaw from mag
				float Yh = (magY * cos(roll)) - (magZ * sin(roll));
				float Xh = (magX * cos(pitch))+(magY * sin(roll)*sin(pitch)) + (magZ * cos(roll) * sin(pitch));
				yaw = gyroZ * 10;

				//filter data
				imu.roll = (Int16S)ToDeg(roll) * -10;
				imu.pitch = (Int16S)ToDeg(pitch) * -10;
			
			
				if(
					(abs(imu.roll) > imu.accthreshold) ||
					(abs(imu.pitch) > imu.accthreshold) 
				)
				{
					if(imu.cbReachThreshold != NULL)
					{
						imu.cbReachThreshold(imu.roll,imu.pitch);
					}
				}
			}
		}		
		//for next time
		prevMillisImuUpdate = DrvTickGetTimeMs();
	}
}


//Get the callback
void SrvImuSimpleSetNotification (cbSrvImuReachThreshold cb) 
{
	imu.cbReachThreshold = cb;
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
void SrvImuSimpleGetRollPitchYaw( Int16S *roll, Int16S *pitch, Int16S *yaw )
{
	roll[0] = imu.roll;
	pitch[0] = imu.pitch;
	yaw[0] = imu.yaw;
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