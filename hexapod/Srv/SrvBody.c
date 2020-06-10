////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "../Drv/DrvTick.h"
#include "../Drv/DrvLed.h"

#include "SrvBody.h"
#include "SrvPID.h"
#include "SrvIHM.h"



////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define BODY_TIMEOUT_INITIALIZATION		5000U	//5 sec
#define BODY_TIMEOUT_BREATH				4000U	//4 sec
////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
static void SrvBodyImuNotify (Int16S roll, Int16S pitch);
static void SrvBodyDetectionNotify (Int8U sensorId, Int16U distance);
////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
SBodyPublic body;


//variables de timming
Int32U lastread_pid = 0U;

Int32U prevMillisUpdateBreath = 0U;
Int32U prevMillisUpdateUS = 0U;
Int32U prevMillisBodyInitialization = 0U;
Int8U prevMillisUpdateBreathStep = 0U;
//Fonction d'initialisation
Boolean SrvBodyInit ( void ) 
{
	
	DrvLegInit();
	body.legs = DrvLegGetLegs();
	
	body.head = SrvHeadGetStruct();
	
	body.battery = SrvBatteryGetStruct();
	
	body.imu = SrvImuSimpleGetSensor();
	SrvImuSimpleSetThreshold(IMU_THRESHOLD_ANGLE);	
	SrvImuSimpleSetNotification(SrvBodyImuNotify);
	
	body.detection = SrvDetectionGetStruct();
	SrvDetectionSetNotification(SrvBodyDetectionNotify);
	
	SrvBodyMoveInit();
	body.move = SrvBodyMoveGetStruct();
	
	body.walk = SrvWalkGetStruct();
	
	body.feeling = SrvFeelingGetFeeling();
	
	//body is not yet initialized
	body.initialized = FALSE;
	
	/*SrvPIDInit();
	SrvPIDSetValues( 0, 0.5F, 0.0F, 0.1F );
	SrvPIDSetValues( 1, 0.5F, 0.0F, 0.1F );*/
	
	//set initial State
	SrvBodyMoveSetGroundSize(65);
	SrvBodyMoveSetElevation(70);
	//at the beginning go to star position
	//SrvBodyMoveSetBehavior(E_BODY_STAR, BODY_TIMEOUT_INITIALIZATION);
	SrvHeadScan();
	prevMillisBodyInitialization = DrvTickGetTimeMs();
	
	return TRUE;
}

//Fonction de dispatching d'evenements
void SrvBodyUpdate (void)
{
	//wait for intializing
	if ((body.initialized == FALSE) && ((DrvTickGetTimeMs() - prevMillisBodyInitialization) > BODY_TIMEOUT_INITIALIZATION))
	{
		body.initialized = TRUE;
	}
	
	//if initialize and stop so breath if not us detection
	if( (body.initialized == TRUE) && 
	    (body.walk->walking == E_WALK_STOP)&& 
	    (body.move->behavior == E_BODY_STAR)  )
	{
		
		SrvBodyMoveSetElevation(60);
		SrvBodyMoveSetGroundSize(65);
		Boolean checkClosedObject = FALSE;
		//stop walk and move body if detection of ultrasound 
		if(!checkClosedObject) 
		{	//no detection	
			if ((DrvTickGetTimeMs() - prevMillisUpdateBreath) > BODY_TIMEOUT_BREATH)
			{
				if(prevMillisUpdateBreathStep == 0)
				{
					SrvBodyMoveSetElevation(SrvBodyMoveGetElevation() - 10);
					SrvBodyMoveSetPosition(E_BODY_STAR, BODY_TIMEOUT_BREATH);
					prevMillisUpdateBreathStep = 1;
				}
				else
				{
					SrvBodyMoveSetElevation(SrvBodyMoveGetElevation() + 10);
					SrvBodyMoveSetPosition(E_BODY_STAR, BODY_TIMEOUT_BREATH);
					prevMillisUpdateBreathStep = 0;
				}
				prevMillisUpdateBreath = DrvTickGetTimeMs();
			}
		}
	}

	/*Int32U interval_pid = DrvTickGetTimeUs() - lastread_pid;
		
	Int16S errorRoll = SrvPIDCompute(0,bodyMove.imu->roll,bodyMove.x,interval_pid);
	Int16S errorPitch = SrvPIDCompute(1,bodyMove.imu->pitch,bodyMove.y,interval_pid);
	SrvBodySetPosition(errorRoll,errorPitch,200);
	//get loop update time
	lastread_pid = DrvTickGetTimeUs();	*/	
	
	//update legs position
	DrvLegUpdate();
	DrvServoUpdate();
}


static void SrvBodyDetectionNotify (Int8U sensorId, Int16U distance)
{
	SrvFeelingSetFeelingLevel(FEELING_STRESS);
}
static void SrvBodyImuNotify (Int16S roll, Int16S pitch)
{
	SrvFeelingSetFeelingLevel(FEELING_STRESS);
}

