/*
 * Srvc
 *
 * Created: 06/07/2012 17:29:19
 *  Author: berryer
 */ 

/////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "SrvImu.h"
#include "SrvPID.h"
#include "SrvSensors.h"

#include "Drv/DrvTick.h"
#include "Drv/DrvEeprom.h"


////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define COEFF_COMPLEMENTARY_FILTER			10.0F

#define NB_SMOOTH_ROLL		5U
#define NB_SMOOTH_PITCH		5U
#define NB_SMOOTH_YAW		5U
////////////////////////////////////////PRIVATE STRUCTURES///////////////////////////////////////

////////////////////////////////////////PRIVATE FONCTIONS/////////////////////////////////////////

static void ComplementaryFilter1orderRoll(float accData, float gyrData, float looptime);
static void ComplementaryFilter1orderPitch(float accData, float gyrData, float looptime);
static void ComplementaryFilter1orderYaw(float magData, float gyrData, float looptime);
////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////

//variables de timming
float intervalSensorFusion = 0.0F;
Int32U lastReadSensorFusion = 0U;

//variables de filtre complementaire d'ordre 2
float readingsRoll[NB_SMOOTH_ROLL];
Int8U indexRoll = 0;
float totalRoll = 0.0F;

float readingsPitch[NB_SMOOTH_PITCH];
Int8U indexPitch = 0;
float totalPitch = 0.0F;

float readingsYaw[NB_SMOOTH_YAW];
Int8U indexYaw = 0;
float totalYaw = 0.0F;


////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
/************************************************************************/
/*Initialisation des composants                                         */
/************************************************************************/
Boolean SrvImuInit( void )
{
	imu.angles.roulis		= 0;
	imu.angles.tangage		= 0;
	imu.angles.lacet		= 0;
	imu.pid_error.altitude  = 0;
	imu.pid_error.roulis	= 0;
	imu.pid_error.tangage	= 0;
	imu.pid_error.lacet		= 0;
	imu.status.armed		= DISARMED;
	for(Int8U loopPID = 0U; loopPID< E_NB_PIDS; loopPID++)
	{
		imu.pid[ loopPID ] = (Pid*)SrvPIDGetStruct(loopPID);
	}
	imu.sensors = SrvSensorsGetStruct();
	imu.moteurs = SrvMotorsGetStruct();
		
	//init of variables
	intervalSensorFusion = 0.0F;
	lastReadSensorFusion = 0U;
	indexRoll = 0;
	totalRoll = 0;
	indexPitch = 0;
	totalPitch = 0;
	indexYaw = 0;
	totalYaw = 0;
	for( Int8U loop = 0U ; loop < NB_SMOOTH_ROLL ; loop ++)
	{
		readingsRoll[loop]	= 0.0F;
	}
	for( Int8U loop = 0U ; loop < NB_SMOOTH_PITCH ; loop ++)
	{
		readingsPitch[loop]	= 0.0F;
	}
	for( Int8U loop = 0U ; loop < NB_SMOOTH_YAW ; loop ++)
	{
		readingsYaw[loop]	= 0.0F;
	}
	
	return TRUE;
}

/************************************************************************/
/*Dispatcher d'evenements                                               */
/************************************************************************/
void SrvImuDispatcher (void)
{
	Int32U time = DrvTickGetTimeUs();
	// ********************* Calcul du temps de cycle *************************
	intervalSensorFusion = (float)((float)(time - lastReadSensorFusion) / (float)1000000.0F);
	lastReadSensorFusion = time;
	
	// ********************* Mise à jour roulis tangage lacet *****************
	ComplementaryFilter1orderRoll(imu.sensors->acc->roulis, imu.sensors->gyr->roulis, intervalSensorFusion);
	ComplementaryFilter1orderPitch(imu.sensors->acc->tangage, imu.sensors->gyr->tangage, intervalSensorFusion);
	ComplementaryFilter1orderYaw(imu.sensors->mag->heading, imu.sensors->gyr->lacet,intervalSensorFusion);
}


/************************************************************************/
/*Enregistre la position de depart                                      */
/************************************************************************/
void SrvImuSensorsSetAltitudeDepart( void )
{
	DrvEepromWriteAltitude(imu.sensors->bar->altitude);
}

/************************************************************************/
/*Enregistre l altitude de maintient                                    */
/************************************************************************/
void SrvImuSensorsSetAltitudeMaintient( Int8U altitude )
{
	if(altitude != 0U)
	{
		Int16S alt = 0U;
		DrvEepromReadAltitude(&alt);
		//imuDesire.maintient_altitude = TRUE;
		//imuDesire.altitude = alt + altitude;
	}
	else
	{
		//imuDesire.maintient_altitude = FALSE;
	}
}


////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
/************************************************************************/
/*filtre complementaire de l'axe de roulis                              */
/************************************************************************/
static void ComplementaryFilter1orderRoll(float accData, float gyrData, float looptime)
{
	static float angleRoll = 0;	
	
	float a = 0.0F;
	float tau = 0.075F;
	
	//accData *= 10U;
	//gyrData *= 10U;
	
	a=tau/(tau+looptime);
	angleRoll = a* (angleRoll + gyrData * looptime) + (1-a) * (accData);
	
	//low pass filter
	totalRoll = totalRoll - readingsRoll[indexRoll];
	readingsRoll[indexRoll] = angleRoll;
	totalRoll = totalRoll + readingsRoll[indexRoll];
	indexRoll = indexRoll + 1;
	if (indexRoll >= NB_SMOOTH_ROLL)
	{
		indexRoll = 0U;
	}
	imu.angles.roulis = (Int16S)(totalRoll / NB_SMOOTH_ROLL);
	//imu.angles.roulis = imu.angles.roulis * 10U;
}


/************************************************************************/
/*filtre complementaire de l'axe de tangage                             */
/************************************************************************/
static void ComplementaryFilter1orderPitch(float accData, float gyrData, float looptime)
{
	static float anglePitch = 0;
	
	float a = 0.0F;
	float tau = 0.075F;
	
	//accData *= 10U;
	//gyrData *= 10U;
	
	a=tau/(tau+looptime);
	anglePitch = a* (anglePitch + gyrData * looptime) + (1-a) * (accData);

	//low pass filter
	totalPitch = totalPitch - readingsPitch[indexPitch];
	readingsPitch[indexPitch] = anglePitch;
	totalPitch = totalPitch + readingsPitch[indexPitch];
	indexPitch = indexPitch + 1;
	if (indexPitch >= NB_SMOOTH_PITCH)
	{
		indexPitch = 0U;
	}
	imu.angles.tangage = (Int16S)(totalPitch / NB_SMOOTH_PITCH);
	//imu.angles.tangage = imu.angles.tangage * 10U;
}

/************************************************************************/
/*filtre complementaire de l'axe de lacet                               */
/************************************************************************/
static void ComplementaryFilter1orderYaw(float magData, float gyrData, float looptime)
{
	static float angleYaw = 0.0F;
	
	float a = 0.0F;
	float tau = 0.075F;
	a=tau/(tau+looptime);
	angleYaw = a* (angleYaw + gyrData * looptime) + (1-a) * (magData);

	//lacet sur modulo 360
	if(angleYaw < -360.0F)
	{
		angleYaw += 360.0F;
	}
	else if(angleYaw > 360.0F)
	{
		angleYaw -= 360.0F;
	}

	//low pass filter
	totalYaw = totalYaw - readingsYaw[indexYaw];
	readingsYaw[indexYaw] = angleYaw;
	totalYaw = totalYaw + readingsYaw[indexYaw];
	indexYaw = indexYaw + 1;
	if (indexYaw >= NB_SMOOTH_YAW)
	{
		indexYaw = 0U;
	}
	imu.angles.lacet = (Int16S)(totalYaw / NB_SMOOTH_YAW);
}


