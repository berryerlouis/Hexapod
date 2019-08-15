/*
* SrvHeartbeat.c
*
* Created: 06/07/2012 16:34:50
*  Author: berryer
*/
/////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvTick.h"
#include "Drv/DrvTwi.h"
#include "Drv/DrvEeprom.h"

#include "Srv/SrvSensors.h"
#include "Srv/SrvImu.h"

////////////////////////////////////////PRIVATE DEFINES/////////////////////////////////////////


////////////////////////////////////////PRIVATE FUNCTIONS////////////////////////////////////////
static void SrvSensorsReadMagnetometerSensor( void );
static void SrvSensorsReadGyroscopeSensor( void );
static void SrvSensorsReadAccelerometerSensor( void );
static void SrvSensorsReadBarometerSensor( void );
static void SrvSensorsReadGPSSensor( void );
static void SrvSensorsReadUltraSonicSensor( void );
static void SrvSensorsReadVariometerSensor( void );
////////////////////////////////////////PRIVATE VARIABLES///////////////////////////////////////

//variables de timming
Int32U lastSensorsLoopTime = 0U;
float intervalGyro = 0.0F;
Int32U lastreadGyro = 0U;
float magnetoCounter = 0.0F;
Int32U magnetoCounterRead = 0U;
float intervalBaro = 0.0F;
Int32U lastreadBaro = 0U;

//available sensors
Sensors sensors;


//Init des valeurs d'allumage moteurs
Boolean SrvSensorsInit( void )
{
	//init driver I2C
	DrvTwiInit( TWI_SPEED_400K );
	
	//raz status
	sensors.status.taskToDo = READ_GPS;
	sensors.status.error = READ_NOTHING;
	
	
	//accelerometer
	if(TRUE == SnrAccelerometerInit(ACC_1G_TO_2G))
	{
		sensors.acc = SnrAccelerometerGetStruct();
	}
	else
	{
		return FALSE;
	}
	
	//gyroscope
	if(TRUE == SnrGyroscopeInit(GYR_2000DPS))
	{
		sensors.gyr = SnrGyroscopeGetStruct();
	}
	else
	{
		return FALSE;
	}
	
	//magnetometer
	if(TRUE == SnrMagnetometerInit(MAG_1_3GA))
	{
		sensors.mag = SnrMagnetometerGetStruct();
	}
	else
	{
		return FALSE;
	}	
	
	//barometer
	if(TRUE == SnrBarometerInit(555U))
	{
		sensors.bar = SnrBarometerGetStruct();
	}
	else
	{
		return FALSE;
	}
	
	//variometer
	if(TRUE == SnrVariometerInit())
	{
		sensors.var = SnrVariometerGetStruct();
	}
	else
	{
		return FALSE;
	}

	//GPS
	if(TRUE == SnrGpsInit())
	{
		sensors.gps = SnrGpsGetStruct();
	}
	else
	{
		return FALSE;
	}	
	
	//US
	if(TRUE == SnrUltraSonicInit())
	{
		sensors.us = SnrUltraSonicGetStruct();
	}
	else
	{
		return FALSE;
	}
	return SrvSensorsSensorsCalibration();
}

//dispatcher d'evenements
void SrvSensorsDispatcher (void)
{
	Int32U time = DrvTickGetTimeUs();
	sensors.status.now = time;
	// ********************* Mesure des capteurs ******************************		
	switch (sensors.status.taskToDo) 
	{
		case READ_GPS:
		{
			//read GPS
			SrvSensorsReadGPSSensor();		
			break;
		}
		case READ_ACC:
		{
			//read acc
			SrvSensorsReadAccelerometerSensor();
			break;
		}
		case READ_GYR:
		{
			//read gyro 
			SrvSensorsReadGyroscopeSensor();
			break;
		}
		case READ_MAG:
		{
			//read mag
			SrvSensorsReadMagnetometerSensor();
			break;
		}
		case READ_BAR:
		{
			//read baro & temp
			SrvSensorsReadBarometerSensor();
			break;
		}
		case READ_VAR:
		{
			//read baro & temp
			SrvSensorsReadVariometerSensor();
			break;
		}
		case READ_US:
		{
			//read us
			SrvSensorsReadUltraSonicSensor();
			break;
		}
		case READ_NOTHING:
		{
			sensors.status.taskToDo = READ_GPS;
			break;
		}
		case READ_NB_TASK:
		{
			sensors.status.taskToDo = READ_GPS;
			break;
		}	
		default:
		{
			sensors.status.taskToDo = READ_GPS;
			break;
		}	
	}
	
	//test timeout for each sensors
	sensors.status.loopTime[ sensors.status.taskToDo ] = time - sensors.status.now;
	if( sensors.status.loopTime[ sensors.status.taskToDo ] > 15000U)
	{
		//enum to bitmap to save all errors
		BIT_HIGH (sensors.status.error, sensors.status.taskToDo);
	}
	
	//loop to the next task
	if( ++sensors.status.taskToDo == READ_NB_TASK )
	{
		sensors.status.taskToDo = READ_GPS;
	}
}



//get the struct of the using sensors
Sensors *SrvSensorsGetStruct ( void ) 
{
	return &sensors; 
}

/************************************************************************/
/*Calibration des capteurs                                              */
/************************************************************************/
Boolean SrvSensorsSensorsCalibration( void )
{
	Boolean oSuccess = FALSE;
	Boolean calibrateAcc = FALSE;
	Boolean calibrateGyr = FALSE;
	Boolean calibrateMag = FALSE;
	
	#if defined( DAISY_7 )
	
		//calib accelerometer
		calibrateAcc = SnrAccelerometerIsCalibrate();
		if( calibrateAcc == FALSE)
		{
			calibrateAcc = SnrAccelerometerSetCalibration();
			SnrAccelerometerSaveCalibration();
		}
		
		//calib gyroscope
		calibrateGyr = SnrGyroscopeIsCalibrate();
		if( calibrateGyr == FALSE)
		{
			calibrateGyr = SnrGyroscopeSetCalibration();
			SnrGyroscopeSaveCalibration();
		}

		//calib magnetometer
		calibrateMag = SnrMagnetometerIsCalibrate() ;
		if( calibrateMag == FALSE)
		{
			calibrateMag = SnrMagnetometerSetCalibration();
			SnrMagnetometerSaveCalibration();
		}
	#endif
	
	oSuccess = (calibrateAcc && calibrateGyr && calibrateMag) ? TRUE : FALSE;
	
	return oSuccess ;
}


////////////////////////////////////////PRIVATE FUNCTIONS////////////////////////////////////////


/************************************************************************/
/*Lecture Accelerometer                                                 */
/************************************************************************/
static void SrvSensorsReadAccelerometerSensor( void )
{
	if( TRUE == SnrAccelerometerGetAcceleration())
	{
		sensors.acc->rawData.x *= -1;
		sensors.acc->rawData.y *= 1;
		sensors.acc->rawData.z *= 1;
	
		//Roll & Pitch
		sensors.acc->roulis = (float)ToDeg(atan2((double)sensors.acc->rawData.y, (double)sensors.acc->rawData.z));
		sensors.acc->tangage = (float)ToDeg(atan2((double)(sensors.acc->rawData.x) , (double)sqrt((double)(pow((double)sensors.acc->rawData.y,2)+pow((double)sensors.acc->rawData.z,2)))));
	}
}

/************************************************************************/
/*Lecture Gyroscope                                                     */
/************************************************************************/
static void SrvSensorsReadGyroscopeSensor( void )
{	
	// ********************* Calcul du temps de cycle *************************
	Int32U now_gyro = DrvTickGetTimeUs();
	intervalGyro = (float)(now_gyro - lastreadGyro) / 1000000.0F;
	lastreadGyro = now_gyro;

	//GYR
	if(SnrGyroscopeGetRotation() != FALSE)
	{
		sensors.gyr->rawData.x *= -1.0;
		sensors.gyr->rawData.y *= -1.0;
		sensors.gyr->rawData.z *= -1.0;
		//scaled
		//*************			
		sensors.gyr->roulis	 += ((float)( sensors.gyr->rawData.x ) * intervalGyro);
		sensors.gyr->tangage += ((float)( sensors.gyr->rawData.y ) * intervalGyro);
		sensors.gyr->lacet   += ((float)( sensors.gyr->rawData.z ) * intervalGyro);
	}
}

/************************************************************************/
/*Lecture Magnetometer                                                  */
/************************************************************************/
static void SrvSensorsReadMagnetometerSensor( void )
{
	Int32U time = DrvTickGetTimeUs();
	magnetoCounter += time - magnetoCounterRead;
	magnetoCounterRead = time;
	//every 100ms
	//if( magnetoCounter > 100000U )
	{
		magnetoCounter = 0U;
		if( TRUE == SnrMagnetometerGetScaleData())
		{
			sensors.mag->rawData.x *= 1;
			sensors.mag->rawData.y *= 1;
			sensors.mag->rawData.z *= 1;
				
			//on doit etre en dessous des 10 deg
			//if(!(imu.angles.roulis > 100 || imu.angles.roulis < -100 || imu.angles.tangage > 100 || imu.angles.tangage < -100))
			{
				//compensation avec l'accelerometre
				/*float cosRoll  = cos( ToRad( angles->tangage ) );
				float sinRoll  = sin( ToRad( angles->tangage ) );
				float cosPitch = cos( ToRad( angles->roulis ) );
				float sinPitch = sin( ToRad( angles->roulis ) );
			
				float Xh = sensors->rawData.x * cosPitch + sensors->rawData.z * sinPitch;
				float Yh = sensors->rawData.x * sinRoll * sinPitch + sensors->rawData.y * cosRoll - sensors->rawData.z * sinRoll * cosPitch;
			
				float heading = -1*atan2(Yh, Xh);
				*/
			
				float heading = atan2(sensors.mag->rawData.y, sensors.mag->rawData.x);
				heading += sensors.mag->declination;
				if(heading < 0)
				{
					heading += 2 * M_PI;
				}
				if(heading > 2 * M_PI)
				{
					heading -= 2 * M_PI;
				}
				sensors.mag->heading = (Int16S)ToDeg(heading);
			}
		}	
	}
}

/************************************************************************/
/*Lecture Baromter                                                      */
/************************************************************************/
static void SrvSensorsReadBarometerSensor( void )
{
	Int32U time = DrvTickGetTimeUs();
	intervalBaro += time - lastreadBaro;
	lastreadBaro = time;
	//every 250ms
	if( intervalBaro > 250000U )
	{
		SnrBarometerUpdate();
		
		//read temperature
		sensors.bar->temperature = (Int16S)SnrBarometerGetTemperature();
	}
}

/************************************************************************/
/*Lecture Ultrasonic													*/
/************************************************************************/
static void SrvSensorsReadUltraSonicSensor( void )
{
	SnrUltraSonicGetDistance();
}

/************************************************************************/
/*Lecture GPS															*/
/************************************************************************/
static void SrvSensorsReadGPSSensor( void )
{
	SnrGpsDecodeGPSFrame();
}

/************************************************************************/
/*Lecture Variometer													*/
/************************************************************************/
static void SrvSensorsReadVariometerSensor( void )
{
	SnrVariometerGetVariation( sensors.bar->altitude );
}
/////////////////////////////////////ISR PRIVATE FUNCTIONS////////////////////////////////////////
