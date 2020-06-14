////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "../Drv/DrvTick.h"
#include "../Drv/DrvLed.h"

#include "SrvTask.h"
#include "SrvPID.h"
#include "SrvIHM.h"



////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define TIMEOUT_INITIALIZATION			6000U	//6 sec

#define TASK_BREATH_ID					0U
#define TASK_DETECTION_ID				1U
#define TASK_SCAN_ID					2U
#define TASK_IMU_ID						3U

#define TASK_BREATH_TIMEOUT				4000U	//4 sec
#define TASK_DETECTION_TIMEOUT			0U		//every loop
#define TASK_SCAN_TIMEOUT				20000U	//10 sec
#define TASK_IMU_TIMEOUT				500U	//500 msec

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
static void SrvTaskImuNotify (Int16S roll, Int16S pitch);
static void SrvTaskDetectionNotify (Boolean reachThreshold);

static void SrvTaskBreath (void);
static void SrvTaskDetection (void);
static void SrvTaskIMU (void);
static void SrvTaskScan (void);
////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////


STask tasks [] =
{
	{
		TASK_BREATH_ID,
		TRUE,
		SrvTaskBreath,
		0U,
		TASK_BREATH_TIMEOUT,
	},
	{
		TASK_DETECTION_ID,
		FALSE,
		SrvTaskDetection,
		0U,
		TASK_DETECTION_TIMEOUT,
	},
	{
		TASK_IMU_ID,
		FALSE,
		SrvTaskIMU,
		0U,
		TASK_IMU_TIMEOUT,
	},
	{
		TASK_SCAN_ID,
		TRUE,
		SrvTaskScan,
		0U,
		TASK_SCAN_TIMEOUT,
	}
};
#define NB_TASK (Int8U)((Int8U)sizeof(tasks)/sizeof(STask))
////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
STaskPublic hexapod;


//variables de timming
Int32U prevMillisBodyInitialization = 0U;

//Fonction d'initialisation
Boolean SrvTasknit ( void ) 
{	
	DrvLegInit();
	
	hexapod.tasks = tasks;
	
	hexapod.legs = DrvLegGetLegs();
	
	hexapod.head = SrvHeadGetStruct();
	
	hexapod.battery = SrvBatteryGetStruct();
	
	hexapod.imu = SrvImuSimpleGetSensor();
	SrvImuSimpleSetThreshold(IMU_THRESHOLD_ANGLE);	
	SrvImuSimpleSetNotification(SrvTaskImuNotify);
	
	hexapod.detection = SrvDetectionGetStruct();
	SrvDetectionSetNotification(SrvTaskDetectionNotify);
	
	SrvBodyMoveInit();
	hexapod.move = SrvBodyMoveGetStruct();
	
	hexapod.walk = SrvWalkGetStruct();
	
	hexapod.feeling = SrvFeelingGetFeeling();
	
	//body is not yet initialized
	hexapod.initialized = FALSE;
	
	/*SrvPIDInit();
	SrvPIDSetValues( 0, 0.5F, 0.0F, 0.1F );
	SrvPIDSetValues( 1, 0.5F, 0.0F, 0.1F );*/
	
	return TRUE;
}

//Fonction de dispatching d'evenements
void SrvTaskUpdate (void)
{
	//wait for intializing
	if ((hexapod.initialized == FALSE) && (hexapod.legs->isInitialized == TRUE) && ((DrvTickGetTimeMs() - prevMillisBodyInitialization) > TIMEOUT_INITIALIZATION))
	{
		//set initial State
		SrvBodyMoveSetGroundSize(65);
		SrvBodyMoveSetElevation(70);
		//set to initial position
		SrvBodyMoveSetPosition(E_BODY_STAR, TIMEOUT_INITIALIZATION);
		//init task time
		for(Int8U i = 0U ; i < NB_TASK ; i++)
		{
			tasks[i].updateTime = DrvTickGetTimeMs();
		}
		
		hexapod.initialized = TRUE;
	}
	else if(hexapod.initialized == TRUE)
	{
		//do tasks
		for(Int8U i = 0U ; i < NB_TASK ; i++)
		{
			if( (tasks[i].enable == TRUE) && (tasks[i].updateTime < DrvTickGetTimeMs()) )
			{
				tasks[i].notify();
				tasks[i].updateTime = DrvTickGetTimeMs() + tasks[i].timeout;
			}
		}
	}
	
	//update legs position
	DrvLegUpdate();
}




void SrvTaskEnable (Int8U taskId)
{
	tasks[taskId].enable = TRUE;
	//reset time
	tasks[taskId].updateTime = DrvTickGetTimeMs();
}

void SrvTaskDisable (Int8U taskId)
{
	tasks[taskId].enable = FALSE;
}





static void SrvTaskBreath (void)
{
	static Boolean toggleBreath = FALSE;
	//only if stopped and star position
	if(	(hexapod.walk->walking == E_WALK_STOP) &&
	    (hexapod.move->behavior == E_BODY_STAR) )
	{
		SrvBodyMoveSetElevation(60);
		SrvBodyMoveSetGroundSize(65);
		
		if(toggleBreath == FALSE)
		{
			SrvBodyMoveSetRelativeElevation(-2);
			SrvBodyMoveSetPosition(E_BODY_STAR, TASK_BREATH_TIMEOUT);
			toggleBreath = TRUE;
		}
		else
		{
			SrvBodyMoveSetRelativeElevation(+2);
			SrvBodyMoveSetPosition(E_BODY_STAR, TASK_BREATH_TIMEOUT);
			toggleBreath = FALSE;
		}
	}
}

static void SrvTaskDetection (void)
{
	//start stressed heartbeat
	SrvFeelingSetFeelingLevel(FEELING_STRESS);
	float roll  = 0.0F;
	float pitch = 0.0F;
	float yaw   = 0.0F;
	float x		= 0.0F;
	float y		= 0.0F;
	float z		= 0.0F;
	
	//try to find angle
	//LEFT US0
	if((hexapod.detection->detect[0] == TRUE) && (hexapod.detection->detect[1] == FALSE) && (hexapod.detection->detect[2] == FALSE))
	{
		SrvHeadSetPosition(HEAD_HORI_MAX,50);
		SrvIhmPlatformLeftLedTimeOn(E_LED_STATE_ON,50);
		roll  = 15/2;
		pitch = 0;
		yaw   = -15;
	}
	//CENTER LEFT US0 + LAZER
	else if((hexapod.detection->detect[0] == TRUE) && (hexapod.detection->detect[1] == FALSE) && (hexapod.detection->detect[2] == TRUE))
	{
		SrvHeadSetPosition(((HEAD_HORI_MAX - HEAD_HORI_MID) / 2)+HEAD_HORI_MID,50);
		SrvIhmPlatformLeftLedTimeOn(E_LED_STATE_ON,50);
		roll  = 15/2;
		pitch = 0;
		yaw   = -15/2;
	}
	//CENTER LAZER OR LAZER + US0 + US1
	else if(((hexapod.detection->detect[0] == TRUE) && (hexapod.detection->detect[1] == TRUE) && (hexapod.detection->detect[2] == TRUE)) ||
			((hexapod.detection->detect[0] == FALSE) && (hexapod.detection->detect[1] == FALSE) && (hexapod.detection->detect[2] == TRUE)))
	{
		SrvHeadSetPosition(HEAD_HORI_MID,50);
		SrvIhmPlatformLeftLedTimeOn(E_LED_STATE_ON,50);
		SrvIhmPlatformRightLedTimeOn(E_LED_STATE_ON,50);
		roll  = 15/2;
		pitch = 0;
		yaw   = 0;
	}
	//CENTER RIGHT US1 + LAZER
	else if((hexapod.detection->detect[0] == FALSE) && (hexapod.detection->detect[1] == TRUE) && (hexapod.detection->detect[2] == TRUE))
	{
		SrvIhmPlatformRightLedTimeOn(E_LED_STATE_ON,50);
		SrvHeadSetPosition(((HEAD_HORI_MIN - HEAD_HORI_MID) / 2)+HEAD_HORI_MID,50);
		roll  = 15/2;
		pitch = 0;
		yaw   = 15/2;
	}
	//RIGHT US1
	else if((hexapod.detection->detect[0] == FALSE) && (hexapod.detection->detect[1] == TRUE) && (hexapod.detection->detect[2] == FALSE))
	{
		SrvHeadSetPosition(HEAD_HORI_MIN,50);
		SrvIhmPlatformRightLedTimeOn(E_LED_STATE_ON,50);
		roll  = 15/2;
		pitch = 0;
		yaw   = 15;
	}
	
	if(hexapod.feeling->state == FEELING_AGRESSIVE)
	{
		yaw *= -1;
		x = 15;
	}
	else if(hexapod.feeling->state == FEELING_NEUTRAL)
	{
		
	}
	else if(hexapod.feeling->state == FEELING_FEAR)
	{
		yaw *= 1;
		x = -15;
	}
	
	SrvBodyMoveSetRotationAndTranslation(roll,pitch,yaw,x,y,z);
	SrvBodyMoveApplyRotationAndTranslation(250);	
}

static void SrvTaskScan (void)
{
	SrvHeadScan();
}

static void SrvTaskIMU (void)
{

	/*
	Int32U lastread_pid = 0U;
	Int32U interval_pid = DrvTickGetTimeUs() - lastread_pid;
		
	Int16S errorRoll = SrvPIDCompute(0,bodyMove.imu->roll,bodyMove.x,interval_pid);
	Int16S errorPitch = SrvPIDCompute(1,bodyMove.imu->pitch,bodyMove.y,interval_pid);
	SrvBodySetPosition(errorRoll,errorPitch,200);
	//get loop update time
	lastread_pid = DrvTickGetTimeUs();	*/	
}










static void SrvTaskDetectionNotify (Boolean reachThreshold)
{
	if(reachThreshold)
	{
		//enable task detection
		SrvTaskEnable(TASK_DETECTION_ID);
		SrvTaskDisable(TASK_BREATH_ID);
		SrvTaskDisable(TASK_SCAN_ID);
		hexapod.head->scanning = FALSE;
	}
	else
	{
		//disable task detection
		SrvTaskDisable(TASK_DETECTION_ID);
		SrvTaskEnable(TASK_BREATH_ID);
		SrvTaskEnable(TASK_SCAN_ID);
	}
	
}
static void SrvTaskImuNotify (Int16S roll, Int16S pitch)
{
	SrvFeelingSetFeelingLevel(FEELING_STRESS);
}

