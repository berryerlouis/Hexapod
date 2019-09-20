////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvTick.h"
#include "Drv/DrvLeg.h"
#include "SrvWalk.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define NB_STEPS	4U
////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

//Stop gaits
static void SrvWalkTripod ( void );
static void SrvWalkWave ( void );
static void SrvWalkRipple ( void );
static void SrvWalkLegsStep( E_LEG movingLeg1, E_LEG movingLeg2, E_LEG movingLeg3,Int16U speed);
static void SrvWalkStop ( void );

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////

E_GAIT gaiting;
E_WALK walking;
Int16U walkingDelay;


Int8U updateLeg		= 0U;

typedef struct
{
	Int8U step;
	Int16U speed;
}S_StepLeg;

S_StepLeg stepLeg[NB_LEGS];
Boolean initStepGaits = FALSE;
Int16S legsXTab	[]= {  15,  45, -30 };
Int16S legsYTab	[]= {  -20,   0,  0 };
Int16S legsZTab	[]= {  30,   0,  0 };



////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////


//Fonction d'initialisation
//return : TRUE si ok
Boolean SrvWalkInit ( void )
{
    Boolean oSuccess = TRUE;
	SrvWalkSetWalk(E_WALK_STOP, 0U);
	SrvWalkSetGait(E_GAITS_TRIPOD, 0U);
    return oSuccess;
}

//Fonction de dispatching d'evenements
void SrvWalkUpdate ( void )
{
	//update servos every 15 ms
	//if ((DrvTickGetTimeMs() - prevMillisWalkUpdate) > 10U)
	if( walking !=  E_WALK_STOP )
	{
		if( gaiting == E_GAITS_TRIPOD )
		{
			SrvWalkTripod();
		}
		else if( gaiting == E_GAITS_WAVE )
		{
			SrvWalkWave();
		}
		else if( gaiting == E_GAITS_RIPPLE )
		{
			SrvWalkRipple();
		}
	}
	else
	{
		SrvWalkStop();
	}

//prevMillisWalkUpdate = DrvTickGetTimeMs();
	//update legs
	DrvLegUpdate();
}

Boolean SrvWalkSetWalk( E_WALK walk, uint16_t delay )
{
    Boolean oSuccess = FALSE;
	
	if(walking != walk)
	{
		walking = walk;
		walkingDelay = delay;
		oSuccess = TRUE;
	}
	else
	{
		walkingDelay = delay;
		oSuccess = TRUE;
	}
    return oSuccess;
}

Boolean SrvWalkSetGait( E_GAIT gait, uint16_t delay )
{
	Boolean oSuccess = FALSE;
	
	if(gaiting != gait)
	{
		gaiting = gait;
		walkingDelay = delay;
		oSuccess = TRUE;
	}
	else
	{
		walkingDelay = delay;
		oSuccess = TRUE;
	}
	return oSuccess;
}








////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

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

static void SrvWalkTripod ( void )
{	
	if(!initStepGaits)
	{
		initStepGaits = TRUE;
		stepLeg[E_LEG_U_L].step = 0;
		stepLeg[E_LEG_M_L].step = 2;
		stepLeg[E_LEG_B_L].step = 0;
		stepLeg[E_LEG_U_R].step = 2;
		stepLeg[E_LEG_M_R].step = 0;
		stepLeg[E_LEG_B_R].step = 2;
	}
	
	SrvWalkLegsStep(E_LEG_U_L,E_LEG_M_R,E_LEG_B_L, walkingDelay);
	SrvWalkLegsStep(E_LEG_U_R,E_LEG_M_L,E_LEG_B_R, walkingDelay);
}	
			 
static void SrvWalkWave ( void )
{
	static E_LEG movingLeg = E_LEG_U_L;
	//Int8U gaitsTabWave		[]= {0,0,0,0,0,1, 
								 //0,0,0,0,1,0,
								 //0,0,0,1,0,0,
								 //0,0,1,0,0,0,
								 //0,1,0,0,0,0,
								 //1,0,0,0,0,0};				
	if(updateLeg == 0U)
	{
		DrvLegSetXYZ(movingLeg,-15,30,-30,walkingDelay/2);
	}
	else if(updateLeg == 1U)
	{
		DrvLegSetXYZ(movingLeg,-45,0,0,walkingDelay);
	}
	else if(updateLeg == 2U)
	{
		DrvLegSetXYZ(movingLeg,30,0,0,walkingDelay*2);
	}
	
	if(
		DrvLegCheckTarget(E_LEG_U_L) &&
		DrvLegCheckTarget(E_LEG_M_L) &&
		DrvLegCheckTarget(E_LEG_B_L) &&
		DrvLegCheckTarget(E_LEG_U_R) &&
		DrvLegCheckTarget(E_LEG_M_R) &&
		DrvLegCheckTarget(E_LEG_B_R)
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
static void SrvWalkRipple ( void )
{
	E_LEG moveLegs[] = {
		E_LEG_U_L,
		E_LEG_M_R,
		E_LEG_B_L,
		E_LEG_U_R,
		E_LEG_M_L,
		E_LEG_B_R,
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
				walkingDelay);
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
		DrvLegCheckTarget(E_LEG_U_L) &&
		DrvLegCheckTarget(E_LEG_M_L) &&
		DrvLegCheckTarget(E_LEG_B_L) &&
		DrvLegCheckTarget(E_LEG_U_R) &&
		DrvLegCheckTarget(E_LEG_M_R) &&
		DrvLegCheckTarget(E_LEG_B_R)
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
//Stop
static void SrvWalkStop ( void )
{
	if(!(DrvLegCheckTarget(E_LEG_U_L) && DrvLegCheckTarget(E_LEG_M_L) && DrvLegCheckTarget(E_LEG_B_L) && 
	   DrvLegCheckTarget(E_LEG_U_R) && DrvLegCheckTarget(E_LEG_M_R) && DrvLegCheckTarget(E_LEG_B_R))
	)
	{
		DrvLegSetPosition(E_LEG_U_L, LEG_COXA_U_L_MID, LEG_FEMUR_U_L_MAX, LEG_TIBIA_U_L_MAX, 0);
		DrvLegSetPosition(E_LEG_M_L, LEG_COXA_M_L_MID, LEG_FEMUR_M_L_MAX, LEG_TIBIA_M_L_MAX, 0);
		DrvLegSetPosition(E_LEG_B_L, LEG_COXA_B_L_MID, LEG_FEMUR_B_L_MAX, LEG_TIBIA_B_L_MAX, 0);
		DrvLegSetPosition(E_LEG_U_R, LEG_COXA_U_R_MID, LEG_FEMUR_U_R_MIN, LEG_TIBIA_U_R_MIN, 0);
		DrvLegSetPosition(E_LEG_M_R, LEG_COXA_M_R_MID, LEG_FEMUR_M_R_MIN, LEG_TIBIA_M_R_MIN, 0);
		DrvLegSetPosition(E_LEG_B_R, LEG_COXA_B_R_MID, LEG_FEMUR_B_R_MIN, LEG_TIBIA_B_R_MIN, 0);
	}
}
