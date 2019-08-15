/*
 * SrvHeartbeat.h
 *
 * Created: 06/07/2012 16:35:15
 *  Author: berryer
 */ 


#ifndef SRVSENSORS_H_
#define SRVSENSORS_H_

#include "Tools/tools_typedefs.h"
#include "Conf/ConfHard.h"

#include "Snrs/SnrAccelerometer.h"
#include "Snrs/SnrGyroscope.h"
#include "Snrs/SnrMagnetometer.h"
#include "Snrs/SnrUltraSonic.h"
#include "Snrs/SnrBarometer.h"
#include "Snrs/SnrVariometer.h"

#if GPS == 1U
	#include "Snrs/SnrGPS.h"
#endif


////////////////////////////////////////PUBLIC ENUM //////////////////////////////////////////////
typedef enum
{
	READ_NOTHING,
	READ_GPS,
	READ_ACC,
	READ_GYR,
	READ_MAG,
	READ_BAR,
	READ_VAR,
	READ_US,
	READ_NB_TASK,
}E_SensorTasks;


////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////

typedef struct
{
	E_SensorTasks taskToDo;
	Int32U loopTime[ READ_NB_TASK ];
	Int32U now;
	E_SensorTasks error;
}StatusSensor;


typedef struct
{
	AccelerometerData *acc;
	GyroscopeData *gyr;
	MagnetometerData *mag;
	UltraSonicData *us;
	GpsData *gps;
	BarometerData *bar;
	VariometerData *var;
	StatusSensor status;
}Sensors;



////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
//Init du hearbeat
Boolean SrvSensorsInit( void ) ;

//dispatcher d'evenements
void SrvSensorsDispatcher (void) ;

//do the sensors calibration
Boolean SrvSensorsSensorsCalibration( void ) ;

//get the struct of the using sensors
Sensors *SrvSensorsGetStruct ( void ) ;

#endif /* SRVSENSORS_H_ */