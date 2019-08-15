/*
 * SrvImu.h
 *
 * Created: 06/07/2012 17:29:08
 *  Author: berryer
 */ 


#ifndef SRVIMU_H_
#define SRVIMU_H_


#include "Conf/ConfHard.h"
#include "Tools/tools_typedefs.h"


#include "Srv/SrvMotors.h"
#include "Srv/SrvSensors.h"
#include "Srv/SrvPID.h"



////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////


typedef struct
{
	Int16S roulis;
	Int16S tangage;
	Int16S lacet;
}Angles;

typedef struct
{
	Boolean	armed;
	Int32U	loopTime;
	Int32U	lifeTime;
	Int16U	i2cErrorsCount;
	Int8U	lastResetCause;
}StatusImu;

typedef struct SS_imu
{
	Pid			*pid[ E_NB_PIDS ];
	Sensors		*sensors;
	Motors		*moteurs;
	Angles		angles;
	ErrorPid	pid_error;
	StatusImu	status;
}Imu;

typedef struct SS_imu_desiree
{
	Angles		angles;
	Int16S		altitude;
	Boolean		maintient_altitude;
}S_imu_desiree;


extern Imu imu;


////////////////////////////////////////PUBLIC FONCTIONS/////////////////////////////////////////
Boolean SrvImuInit( void );
void SrvImuDispatcher (void);
void SrvImuSensorsCalibration( void );
void SrvImuSensorsSetAltitudeDepart( void );
void SrvImuSensorsSetAltitudeMaintient( Int8U altitude );

#endif /* DRVIMU_H_ */