////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvLeg.h"
#include "Srv/SrvWalk.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
static void SrvWalkPlayingGaits ( Int16U speed );
//Stop gaits
static Boolean SrvWalkStopGaits ( void );
static void SrvWalkTripodGaits ( Int16U speed );
static void SrvWalkWaveGaits ( Int16U speed );
static void SrvWalkRippleGaits ( Int16U speed );
static void SrvWalkLegsStep( E_LEG movingLeg1, E_LEG movingLeg2, E_LEG movingLeg3,Int16U speed);

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
static volatile E_WALK walk;
E_GAITS gaits;
Int8U updateLeg		= 0U;

typedef struct
{
	Int8U step;
	Int16U speed;
}S_StepLeg;

S_StepLeg stepLeg[NB_LEGS];
Boolean initStepGaits = FALSE;
Int16S legsXTab	[]= { -15, -45, 30 };
Int16S legsYTab	[]= {  50,   0,  0 };
Int16S legsZTab	[]= { -30,   0,  0 };

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////


//Fonction d'initialisation
//return : TRUE si ok
Boolean SrvWalkInit ( void )
{
    Boolean oSuccess = TRUE;
	//init the walking status
	walk = E_WALK_STOP;
	gaits = E_GAITS_TRIPOD;
	SrvWalkStopGaits();

    return oSuccess;
}

//Fonction de dispatching d'evenements
void SrvWalkUpdate ( void )
{
	//if not stop
	/*if(walk != E_WALK_STOP)
	{
		SrvWalkPlayingGaits(500);
	}
	else
	{
		if(SrvWalkStopGaits() == TRUE)
		{
			SrvWalkChangeWalk(E_WALK_STEP);
			SrvWalkChangeGaits(E_GAITS_TRIPOD);
		}
	}*/
	//update legs
	DrvLegUpdate();
}

//Change the status of the walk
//return : TRUE si ok
Boolean SrvWalkChangeWalk ( E_WALK w )
{
	Boolean oSuccess = TRUE;
	//set the walking status
	walk = w;
	return oSuccess;
}

//Change the status of the walk
//return : TRUE si ok
Boolean SrvWalkChangeGaits ( E_GAITS g)
{
	Boolean oSuccess = TRUE;
	//set the walking status
	gaits = g;
	initStepGaits = FALSE;
	return oSuccess;
}

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
//Play gaits
static void SrvWalkPlayingGaits ( Int16U speed )
{	
	if( gaits == E_GAITS_TRIPOD )
	{
		SrvWalkTripodGaits(speed);
	}
	else if( gaits == E_GAITS_WAVE )
	{
		SrvWalkWaveGaits(speed);
	}
	else if( gaits == E_GAITS_RIPPLE )
	{
		SrvWalkRippleGaits(speed);
	}
	else
	{

	}
}

static void SrvWalkLegsStep( E_LEG movingLeg1, E_LEG movingLeg2, E_LEG movingLeg3,Int16U speed)
{
	//decrease delay per 2 for the first movement
	if(stepLeg[movingLeg1].step == 0)speed = (speed * 2U) / 3U;
	//increase delay times 2 for the third movement
	if(stepLeg[movingLeg1].step == 2)speed = speed * 2U;
	
	DrvLegSetXYZ(movingLeg1,
				legsXTab[stepLeg[movingLeg1].step],
				legsYTab[stepLeg[movingLeg1].step],
				legsZTab[stepLeg[movingLeg1].step],
				speed);
	DrvLegSetXYZ(movingLeg2,
				legsXTab[stepLeg[movingLeg2].step],
				legsYTab[stepLeg[movingLeg2].step],
				legsZTab[stepLeg[movingLeg2].step],
				speed);
	DrvLegSetXYZ(movingLeg3,
				legsXTab[stepLeg[movingLeg3].step],
				legsYTab[stepLeg[movingLeg3].step],
				legsZTab[stepLeg[movingLeg3].step],
				speed);
	if(DrvLegCheckTarget(movingLeg1) && DrvLegCheckTarget(movingLeg2) && DrvLegCheckTarget(movingLeg3))
	{
		stepLeg[movingLeg1].step++;
		stepLeg[movingLeg2].step++;
		stepLeg[movingLeg3].step++;
		if((stepLeg[movingLeg1].step) == 3)stepLeg[movingLeg1].step = 0;
		if((stepLeg[movingLeg2].step) == 3)stepLeg[movingLeg2].step = 0;
		if((stepLeg[movingLeg3].step) == 3)stepLeg[movingLeg3].step = 0;
	}
}

static void SrvWalkTripodGaits ( Int16U speed )
{	
	if(!initStepGaits)
	{
		initStepGaits = TRUE;
		stepLeg[E_LEG_L_F].step = 0;
		stepLeg[E_LEG_L_M].step = 2;
		stepLeg[E_LEG_L_R].step = 0;
		stepLeg[E_LEG_R_F].step = 2;
		stepLeg[E_LEG_R_M].step = 0;
		stepLeg[E_LEG_R_R].step = 2;
	}
	
	SrvWalkLegsStep(E_LEG_L_F,E_LEG_R_M,E_LEG_L_R, speed);
	SrvWalkLegsStep(E_LEG_R_F,E_LEG_L_M,E_LEG_R_R, speed);
}	
			 
static void SrvWalkWaveGaits ( Int16U speed )
{
	static E_LEG movingLeg = E_LEG_L_F;
	//Int8U gaitsTabWave		[]= {0,0,0,0,0,1, 
								 //0,0,0,0,1,0,
								 //0,0,0,1,0,0,
								 //0,0,1,0,0,0,
								 //0,1,0,0,0,0,
								 //1,0,0,0,0,0};				
	if(updateLeg == 0U)
	{
		DrvLegSetXYZ(movingLeg,-15,30,-30,speed/2);
	}
	else if(updateLeg == 1U)
	{
		DrvLegSetXYZ(movingLeg,-45,0,0,speed);
	}
	else if(updateLeg == 2U)
	{
		DrvLegSetXYZ(movingLeg,30,0,0,speed*2);
	}
	
	if(
		DrvLegCheckTarget(E_LEG_L_F) &&
		DrvLegCheckTarget(E_LEG_L_M) &&
		DrvLegCheckTarget(E_LEG_L_R) &&
		DrvLegCheckTarget(E_LEG_R_F) &&
		DrvLegCheckTarget(E_LEG_R_M) &&
		DrvLegCheckTarget(E_LEG_R_R)
	)
	{
		updateLeg++;
		if(updateLeg == 3)
		{
			updateLeg = 0;
			movingLeg++;
			if(movingLeg == E_NB_LEGS)
			{
				movingLeg = 0;
			}
		}
	}
}
	
Int8U movingLegIndex = 0;
static void SrvWalkRippleGaits ( Int16U speed )
{
	E_LEG moveLegs[] = {
		E_LEG_L_F,
		E_LEG_R_M,
		E_LEG_L_R,
		E_LEG_R_F,
		E_LEG_L_M,
		E_LEG_R_R,
	};
	//Int8U gaitsTabRipple	[]= {	//1,1,0,0,0,0, 
									//0,0,0,0,1,1,
									//0,0,1,1,0,0,
									//0,0,0,1,1,0,
									//0,1,1,0,0,0,
									//1,0,0,0,0,1};
									

	DrvLegSetXYZ(moveLegs[movingLegIndex],
				legsXTab[stepLeg[moveLegs[movingLegIndex]].step],
				legsYTab[stepLeg[moveLegs[movingLegIndex]].step],
				legsZTab[stepLeg[moveLegs[movingLegIndex]].step],
				speed);
	if(DrvLegCheckTarget(moveLegs[movingLegIndex]))
	{
		stepLeg[moveLegs[movingLegIndex]].step++;
		if((stepLeg[moveLegs[movingLegIndex]].step) == 3)
		{
			movingLegIndex++;
			stepLeg[moveLegs[movingLegIndex]].step = 0;
		}
	}
	
	if(
		DrvLegCheckTarget(E_LEG_L_F) &&
		DrvLegCheckTarget(E_LEG_L_M) &&
		DrvLegCheckTarget(E_LEG_L_R) &&
		DrvLegCheckTarget(E_LEG_R_F) &&
		DrvLegCheckTarget(E_LEG_R_M) &&
		DrvLegCheckTarget(E_LEG_R_R)
	)
	{
		updateLeg++;
		if(updateLeg == 3)
		{
			updateLeg = 0;
			movingLegIndex++;
			if(movingLegIndex == E_NB_LEGS)
			{
				movingLegIndex = 0;
			}
		}
	}
}

//Stop gaits
static Boolean SrvWalkStopGaits ( void )
{
	DrvLegSetXYZ(E_LEG_L_F,0,0,0,100);
	DrvLegSetXYZ(E_LEG_L_M,0,0,0,100);
	DrvLegSetXYZ(E_LEG_L_R,0,0,0,100);
	DrvLegSetXYZ(E_LEG_R_F,0,0,0,100);
	DrvLegSetXYZ(E_LEG_R_M,0,0,0,100);
	DrvLegSetXYZ(E_LEG_R_R,0,0,0,100);

	if( 
		DrvLegCheckTarget(E_LEG_L_F) &&
		DrvLegCheckTarget(E_LEG_L_M) &&
		DrvLegCheckTarget(E_LEG_L_R) &&
		DrvLegCheckTarget(E_LEG_R_F) &&
		DrvLegCheckTarget(E_LEG_R_M) &&
		DrvLegCheckTarget(E_LEG_R_R) 
		)
	{
		return TRUE;
	}
	return FALSE;
}