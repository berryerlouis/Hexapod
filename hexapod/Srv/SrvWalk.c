////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvTick.h"
#include "Drv/DrvLeg.h"
#include "SrvWalk.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define NB_WALK_STEPS	4U
////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

//Stop gaits
static void SrvWalkTripod ( E_WALK walk );
static void SrvWalkWave ( E_WALK walk );
static void SrvWalkRipple ( E_WALK walk );
static void SrvWalkLegsStep( E_LEG movingLeg1, E_LEG movingLeg2, E_LEG movingLeg3,Int16U speed);
static void SrvWalkStop ( void );


static Boolean SrvWalkMoveLeg ( E_LEG movingLeg, Int16U speed );

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////

E_GAIT gaiting = E_GAITS_WAVE;
E_WALK walking = E_WALK_STOP;
Int16U walkingDelay = 1000U;

//period of step
uint32_t prevMillisWalkUpdate = 0U;
Int8U updateLeg		= 0U;

typedef struct
{
	Int8U wait;
	Int8U step;
	Int16U speed;
}S_StepLeg;

S_StepLeg stepLeg[NB_LEGS];

Int16S legsXTripodLeft	[]= { 5,	5,		-5, -5 };
Int16S legsYTripodLeft	[]= { -25,	-40,	-40, -25 };
Int16S legsZTripodLeft	[]= { 20,	30, 30, 20 };

Int16S legsXTripodRight[]= { 	-5, -5   ,5,		5,	 };
Int16S legsYTripodRight[]= { 	15, 30   ,30,	15,	 };
Int16S legsZTripodRight[]= { 	-20, -30 ,-30,	-20,};


////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////

//Fonction d'initialisation
//return : TRUE si ok
Boolean SrvWalkInit ( void )
{
    Boolean oSuccess = TRUE;
	DrvLegInit();
	SrvWalkSetWalk(E_WALK_STOP, 500U);
	SrvWalkSetGait(E_GAITS_TRIPOD, 500U);
    return oSuccess;
}

//Fonction de dispatching d'evenements
void SrvWalkUpdate ( void )
{
	//only when legs are inittialized
	if(DrvLegIsInitialized())
	{
		//only if different STOP
		if( walking !=  E_WALK_STOP )
		{
			//update walk step
			if ((DrvTickGetTimeMs() - prevMillisWalkUpdate) > walkingDelay)
			{
				if( gaiting == E_GAITS_TRIPOD )
				{
					SrvWalkTripod(walking);
				}
				else if( gaiting == E_GAITS_WAVE )
				{
					SrvWalkWave(walking);
				}
				else if( gaiting == E_GAITS_RIPPLE )
				{
					//SrvWalkRipple(walking);
				}
				//for next time
				prevMillisWalkUpdate = DrvTickGetTimeMs();
			}
		}
		else
		{
			SrvWalkStop();
		}
	}
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
		if( gaiting == E_GAITS_TRIPOD )
		{
			stepLeg[E_LEG_U_L].step = 0;
			stepLeg[E_LEG_M_L].step = 2;
			stepLeg[E_LEG_B_L].step = 0;
			stepLeg[E_LEG_U_R].step = 2;
			stepLeg[E_LEG_M_R].step = 0;
			stepLeg[E_LEG_B_R].step = 2;
		}
		else if( gaiting == E_GAITS_WAVE )
		{
			stepLeg[E_LEG_U_L].step = 0;
			stepLeg[E_LEG_M_L].step = 0;
			stepLeg[E_LEG_B_L].step = 0;
			stepLeg[E_LEG_U_R].step = 0;
			stepLeg[E_LEG_M_R].step = 0;
			stepLeg[E_LEG_B_R].step = 0;
		}
		else
		{
			
		}
		
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
	//if(stepLeg[movingLeg1].step == 0)speed = (speed * 2U) / 3U;
	//increase delay times 2 for the third movement
	//if(stepLeg[movingLeg1].step == 2)speed = speed * 2U;
	
	/*DrvLegSetXYZ(movingLeg1,
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
	}*/
}

static void SrvWalkTripod ( E_WALK walk )
{	
	
	if( walk == E_WALK_FW )
	{
		for(E_LEG indexLeg = 0 ; indexLeg < E_NB_LEGS ; indexLeg++)
		{
			Int16U speed = walkingDelay; 
			if((stepLeg[indexLeg].step) != 2 )
			{
				speed = walkingDelay / 4;
			}
			if(SrvWalkMoveLeg(indexLeg,speed))
			{
				stepLeg[indexLeg].step++;
				if((stepLeg[indexLeg].step) == NB_WALK_STEPS )
				{
					stepLeg[indexLeg].step = 0U;
				}
			}
		}
	}
	else
	{
		for(E_LEG indexLeg = 0 ; indexLeg < E_NB_LEGS ; indexLeg++)
		{
			Int16U speed = walkingDelay; 
			if((stepLeg[indexLeg].step) != 1 )
			{
				speed = walkingDelay / 4;
			}
			if(SrvWalkMoveLeg(indexLeg,speed))
			{
				if((stepLeg[indexLeg].step) == 0U )
				{
					stepLeg[indexLeg].step = NB_WALK_STEPS;
				}
				stepLeg[indexLeg].step--;
			}
		}
	}
	
	
}	











static void SrvWalkWave ( E_WALK walk )
{
	static executeAll = FALSE;
	static E_LEG movingLeg = E_LEG_U_L;
	//Int8U gaitsTabWave		[]= {0,0,0,0,0,1, 
								 //0,0,0,0,1,0,
								 //0,0,0,1,0,0,
								 //0,0,1,0,0,0,
								 //0,1,0,0,0,0,
								 //1,0,0,0,0,0};				
	
	if( walk == E_WALK_FW )
	{
		if(SrvWalkMoveLeg(movingLeg, walkingDelay/2))
		{
			if(executeAll == TRUE )
			{
				SrvWalkMoveLeg(E_LEG_U_L, walkingDelay);
				SrvWalkMoveLeg(E_LEG_M_L, walkingDelay);
				SrvWalkMoveLeg(E_LEG_B_L, walkingDelay);
				SrvWalkMoveLeg(E_LEG_U_R, walkingDelay);
				SrvWalkMoveLeg(E_LEG_M_R, walkingDelay);
				SrvWalkMoveLeg(E_LEG_B_R, walkingDelay);
				executeAll = FALSE;
			}
			stepLeg[movingLeg].step++;
			if((stepLeg[movingLeg].step) == 2U )
			{
				movingLeg++;
				if(movingLeg == E_NB_LEGS)
				{
					movingLeg = E_LEG_U_L;
					executeAll = TRUE;
				}
			}
			if((stepLeg[movingLeg].step) == NB_WALK_STEPS )
			{
				stepLeg[movingLeg].step = 0U;
			}
		}
	}
	else if( walk == E_WALK_RV )
	{
		if(SrvWalkMoveLeg(movingLeg, walkingDelay/2))
		{			
			if((stepLeg[movingLeg].step) == 2U )
			{
				movingLeg++;
				if(movingLeg == E_NB_LEGS)
				{
					movingLeg = E_LEG_U_L;
					executeAll = TRUE;
					
					SrvWalkMoveLeg(E_LEG_U_L, walkingDelay);
					SrvWalkMoveLeg(E_LEG_M_L, walkingDelay);
					SrvWalkMoveLeg(E_LEG_B_L, walkingDelay);
					SrvWalkMoveLeg(E_LEG_U_R, walkingDelay);
					SrvWalkMoveLeg(E_LEG_M_R, walkingDelay);
					SrvWalkMoveLeg(E_LEG_B_R, walkingDelay);
				}
			}
			if((stepLeg[movingLeg].step) == 0U )
			{
				stepLeg[movingLeg].step = NB_WALK_STEPS;
			}
			stepLeg[movingLeg].step--;
			if(executeAll == TRUE )
			{
				executeAll = FALSE;
			}
		}
	}
}









	
Int8U movingLegIndex = 0;
static void SrvWalkRipple ( E_WALK walk )
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
									

	/*DrvLegSetXYZ(moveLegs[movingLegIndex],
				legsXTab[stepLeg[moveLegs[movingLegIndex]].step],
				legsYTab[stepLeg[moveLegs[movingLegIndex]].step],
				legsZTab[stepLeg[moveLegs[movingLegIndex]].step],
				walkingDelay);*/
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
	if(	
	((DrvLegCheckTarget(E_LEG_U_L) &&
	 DrvLegCheckTarget(E_LEG_M_L) &&
	 DrvLegCheckTarget(E_LEG_B_L) && 
	 DrvLegCheckTarget(E_LEG_U_R) &&
	 DrvLegCheckTarget(E_LEG_M_R) &&
	 DrvLegCheckTarget(E_LEG_B_R))
	 ))
	{
		DrvLegSetPosition(E_LEG_U_L, LEG_COXA_U_L_MID, LEG_FEMUR_U_L_MID, LEG_TIBIA_U_L_MAX, 500);
		DrvLegSetPosition(E_LEG_M_L, LEG_COXA_M_L_MID, LEG_FEMUR_M_L_MID, LEG_TIBIA_M_L_MAX, 500);
		DrvLegSetPosition(E_LEG_B_L, LEG_COXA_B_L_MID, LEG_FEMUR_B_L_MID, LEG_TIBIA_B_L_MAX, 500);
		DrvLegSetPosition(E_LEG_U_R, LEG_COXA_U_R_MID, LEG_FEMUR_U_R_MID, LEG_TIBIA_U_R_MIN, 500);
		DrvLegSetPosition(E_LEG_M_R, LEG_COXA_M_R_MID, LEG_FEMUR_M_R_MID, LEG_TIBIA_M_R_MIN, 500);
		DrvLegSetPosition(E_LEG_B_R, LEG_COXA_B_R_MID, LEG_FEMUR_B_R_MID, LEG_TIBIA_B_R_MIN, 500);
	}
}



static Boolean SrvWalkMoveLeg ( E_LEG movingLeg, Int16U speed )
{
	
	Int8U indexHanche = 0U + (movingLeg * 3U);
	Int8U indexGenoux  = 1U + (movingLeg * 3U);
	Int8U indexCheville = 2U + (movingLeg * 3U);
	
	//set delay
	stepLeg[movingLeg].speed = speed;
	//set servos position
	uint8_t reponse = 0;
	if(movingLeg < E_LEG_U_R)
	{
		if(movingLeg == E_LEG_U_L)
		{
			reponse = DrvServoSetTarget(indexHanche,	DrvServoGetStruture(indexHanche)->mid + legsXTripodLeft[stepLeg[movingLeg].step] + 20,	stepLeg[movingLeg].speed );
		}
		else if(movingLeg == E_LEG_B_L)
		{
			reponse = DrvServoSetTarget(indexHanche,	DrvServoGetStruture(indexHanche)->mid + legsXTripodLeft[stepLeg[movingLeg].step] - 20,	stepLeg[movingLeg].speed );
		}
		else
		{
			reponse = DrvServoSetTarget(indexHanche,	DrvServoGetStruture(indexHanche)->mid + legsXTripodLeft[stepLeg[movingLeg].step] ,	stepLeg[movingLeg].speed );
		}
		reponse *= 10U;
		reponse += DrvServoSetTarget(indexGenoux,	DrvServoGetStruture(indexGenoux)->mid + legsYTripodLeft[stepLeg[movingLeg].step],	stepLeg[movingLeg].speed );
		reponse *= 10U;
		reponse += DrvServoSetTarget(indexCheville,	DrvServoGetStruture(indexCheville)->mid + legsZTripodLeft[stepLeg[movingLeg].step],	stepLeg[movingLeg].speed );
	}
	else
	{
		if(movingLeg == E_LEG_U_R)
		{
			reponse = DrvServoSetTarget(indexHanche,	DrvServoGetStruture(indexHanche)->mid + legsXTripodRight[stepLeg[movingLeg].step] - 20,	stepLeg[movingLeg].speed );
		}
		else if(movingLeg == E_LEG_B_R)
		{
			reponse = DrvServoSetTarget(indexHanche,	DrvServoGetStruture(indexHanche)->mid + legsXTripodRight[stepLeg[movingLeg].step] + 20,	stepLeg[movingLeg].speed );
		}
		else
		{
			reponse = DrvServoSetTarget(indexHanche,	DrvServoGetStruture(indexHanche)->mid + legsXTripodRight[stepLeg[movingLeg].step] ,	stepLeg[movingLeg].speed );
		}
		reponse *= 10U;
		reponse += DrvServoSetTarget(indexGenoux,	DrvServoGetStruture(indexGenoux)->mid + legsYTripodRight[stepLeg[movingLeg].step],	stepLeg[movingLeg].speed );
		reponse *= 10U;
		reponse += DrvServoSetTarget(indexCheville,	DrvServoGetStruture(indexCheville)->mid + legsZTripodRight[stepLeg[movingLeg].step], stepLeg[movingLeg].speed );
	}
	if(reponse == 111U)
	{
		return TRUE;
	}
	return FALSE;	
}