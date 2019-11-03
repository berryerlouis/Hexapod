////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvServo.h"
#include "Drv/DrvLeg.h"

#include "Cmps/CmpMPU9150.h"

#include "SrvBody.h"
#include "SrvWalk.h"
#include "SrvFeeling.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
uint8_t shoveThreshold = 50U;
////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
extern struct SBody body;


//Fonction d'initialisation
Boolean SrvBodyInit ( void ) 
{
	DrvLegInit();
	
	body.legs = DrvLegGetLegs();
	body.battery = SrvBatteryGetStruct();
	
	CmpMPU9150Init(MPU9150_ADDRESS);
	
	return TRUE;
}

//Fonction de dispatching d'evenements
void SrvBodyUpdate (void)
{
	CmpMPU9150ReadAcc(MPU9150_ADDRESS, &body.acc);
	CmpMPU9150ReadGyr(MPU9150_ADDRESS, &body.gyr);
	CmpMPU9150ReadTmp(MPU9150_ADDRESS, &body.tmp);
	
	//check if hexapod is shoving
	if(CmpMPU9150IsInitialized())
	{
		if(
		(abs(body.acc.accX) > shoveThreshold) ||
		(abs(body.acc.accY) > shoveThreshold) 
		)
		{
			SrvFeelingSetFeeling(FEELING_STRESS);
		}
	}
}
