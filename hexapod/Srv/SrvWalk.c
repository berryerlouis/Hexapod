////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvTick.h"
#include "Drv/DrvLeg.h"
#include "SrvWalk.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define NB_WALK_STEPS_TRIPOD	6U
#define NB_WALK_STEPS_WAVE		9U

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

//Stop gaits
static void SrvWalkTripod ( E_WALK walk );
static void SrvWalkWave ( E_WALK walk );
static void SrvWalkRipple ( E_WALK walk );
static void SrvWalkLegsStep( E_LEG movingLeg1, E_LEG movingLeg2, E_LEG movingLeg3,Int16U speed);
static void SrvWalkStop ( void );


static Boolean SrvWalkMoveLeg ( E_LEG movingLeg, Int8U step, Int16U speed );

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////

E_GAIT gaiting = E_GAITS_WAVE;
E_WALK walking = E_WALK_STOP;
Boolean isStopped = FALSE;
Int16U walkingDelay = 1000U;

//period of step
uint32_t prevMillisWalkUpdate = 0U;
Int8U updateLeg		= 0U;

Int16S legsXTripodLeft	[]= { 5,	5,		-5, -5 };
Int16S legsYTripodLeft	[]= { -25,	-40,	-40, -25 };
Int16S legsZTripodLeft	[]= { 20,	30, 30, 20 };

Int16S legsXTripodRight[]= { 	-5, -5   ,5,		5,	 };
Int16S legsYTripodRight[]= { 	15, 30   ,30,	15,	 };
Int16S legsZTripodRight[]= { 	-20, -30 ,-30,	-20,};

static Int8U stepLegsTripod[NB_WALK_STEPS_TRIPOD][2U] = {
	{0U,2U},
	{1U,2U},
	{1U,3U},
	{2U,0U},
	{2U,1U},
	{3U,1U}};
							
static Int8U stepLegsWave[NB_WALK_STEPS_WAVE][NB_LEGS] = {
						{1U,0U,3U,2U,2U,2U},
						{1U,1U,0U,3U,2U,2U},
						{1U,1U,1U,0U,3U,2U},
						{1U,1U,1U,1U,0U,3U},
						{1U,1U,1U,1U,1U,0U},
						{1U,1U,1U,1U,1U,1U},
		
						{2U,2U,2U,2U,2U,2U},
		
						{3U,2U,2U,2U,2U,2U},
						{0U,3U,2U,2U,2U,2U}};
									
static Int8U sequenceStepWave = 0U;
static Int8U sequenceStepTripod = 0U;
static Int8U evenLegs[3U] = {0,2,4};
static Int8U oddLegs[3U] = {1,3,5};

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////

//Fonction d'initialisation
//return : TRUE si ok
Boolean SrvWalkInit ( void )
{
    Boolean oSuccess = TRUE;
	DrvLegInit();
	sequenceStepWave = 0U;
	sequenceStepTripod = 0U;
	isStopped = FALSE;
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
			//bot moves
			isStopped = FALSE;
			//update walk step
			//if ((DrvTickGetTimeMs() - prevMillisWalkUpdate) >= walkingDelay)
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
				//prevMillisWalkUpdate = DrvTickGetTimeMs();
			}
		}
		else
		{
			//do it once
			if(isStopped == FALSE)
			{
				SrvWalkStop();
				//bot stopped
				isStopped = TRUE;
			}
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
		sequenceStepWave = 0U;
		if( walk == E_WALK_FW )
		{
			sequenceStepTripod=0U;
		}
		if( walk == E_WALK_RV )
		{
			sequenceStepTripod=5U;
		}
		oSuccess = TRUE;
	}
	else
	{
		walkingDelay = delay;
		oSuccess = TRUE;
	}
	isStopped = FALSE;
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
	Int8U success = 0U;
	Boolean nextStep = FALSE;
	
	if( walk == E_WALK_FW )
	{
		switch (sequenceStepTripod)
		{
			case 0://{0U,2U},
				for(Int8U i = 0U ; i < 3U ; i++)
				{
					success |= (SrvWalkMoveLeg(evenLegs[i],stepLegsTripod[0][0], walkingDelay ) << (i*2U));
					success |= (SrvWalkMoveLeg(oddLegs[i],stepLegsTripod[0][1], walkingDelay * 3U)  << ((i*2U) + 1U));
				}
				if((success & 0x15U) == 0x15U)
				{
					nextStep = TRUE;
				}
			break;
			
			case 1://{1U,2U},
				for(Int8U i = 0U ; i < 3U ; i++)
				{
					success |= (SrvWalkMoveLeg(evenLegs[i],stepLegsTripod[1][0], walkingDelay ) << (i*2U));
					success |= (SrvWalkMoveLeg(oddLegs[i],stepLegsTripod[1][1], walkingDelay * 3U) << ((i*2U) + 1U));
				}
				if(success == 0x3FU) 
				{
					nextStep = TRUE;
				}
			break;
			
			case 2://{1U,3U},
				for(Int8U i = 0U ; i < 3U ; i++)
				{
					success |= (SrvWalkMoveLeg(evenLegs[i],stepLegsTripod[2][0], walkingDelay ) << (i*2U));
					success |= (SrvWalkMoveLeg(oddLegs[i],stepLegsTripod[2][1], walkingDelay ) << ((i*2U) + 1U));
				}
				if(success == 0x3FU) 
				{
					nextStep = TRUE;
				}
			break;
			
			case 3://{2U,0U},
				for(Int8U i = 0U ; i < 3U ; i++)
				{
					success |= (SrvWalkMoveLeg(evenLegs[i],stepLegsTripod[3][0], walkingDelay * 3U ) << (i*2U));
					success |= (SrvWalkMoveLeg(oddLegs[i],stepLegsTripod[3][1], walkingDelay ) << ((i*2U) + 1U));
				}
				if((success & 0x2AU) == 0x2AU) 
				{
					nextStep = TRUE;
				}
			break;
			
			case 4://{2U,1U},
				for(Int8U i = 0U ; i < 3U ; i++)
				{
					success |= (SrvWalkMoveLeg(evenLegs[i],stepLegsTripod[4][0], walkingDelay * 3U ) << (i*2U));
					success |= (SrvWalkMoveLeg(oddLegs[i],stepLegsTripod[4][1], walkingDelay ) << ((i*2U) + 1U));
				}
				if(success == 0x3FU) 
				{
					nextStep = TRUE;
				}
			break;
			
			case 5://{3U,1U},
				for(Int8U i = 0U ; i < 3U ; i++)
				{
					success |= (SrvWalkMoveLeg(evenLegs[i],stepLegsTripod[5][0], walkingDelay ) << (i*2U));
					success |= (SrvWalkMoveLeg(oddLegs[i],stepLegsTripod[5][1], walkingDelay ) << ((i*2U) + 1U));
				}
				if(success == 0x3FU) 
				{
					sequenceStepTripod = 0;
				}
			break;
			default : 
			break;
		}
	}
	else if( walk == E_WALK_RV )
	{
		switch (sequenceStepTripod)
		{
			case 0://{0U,2U},
				for(Int8U i = 0U ; i < 3U ; i++)
				{
					success |= (SrvWalkMoveLeg(evenLegs[i],stepLegsTripod[0][0], walkingDelay ) << (i*2U));
					success |= (SrvWalkMoveLeg(oddLegs[i],stepLegsTripod[0][1], walkingDelay * 3U)  << ((i*2U) + 1U));
				}
				if((success & 0x15U) == 0x15U)
				{
					nextStep = TRUE;
				}
			break;
			
			case 1://{1U,2U},
				for(Int8U i = 0U ; i < 3U ; i++)
				{
					success |= (SrvWalkMoveLeg(evenLegs[i],stepLegsTripod[1][0], walkingDelay ) << (i*2U));
					success |= (SrvWalkMoveLeg(oddLegs[i],stepLegsTripod[1][1], walkingDelay * 3U) << ((i*2U) + 1U));
				}
				if(success == 0x3FU) 
				{
					nextStep = TRUE;
				}
			break;
			
			case 2://{1U,3U},
				for(Int8U i = 0U ; i < 3U ; i++)
				{
					success |= (SrvWalkMoveLeg(evenLegs[i],stepLegsTripod[2][0], walkingDelay ) << (i*2U));
					success |= (SrvWalkMoveLeg(oddLegs[i],stepLegsTripod[2][1], walkingDelay ) << ((i*2U) + 1U));
				}
				if(success == 0x3FU) 
				{
					nextStep = TRUE;
				}
			break;
			
			case 3://{2U,0U},
				for(Int8U i = 0U ; i < 3U ; i++)
				{
					success |= (SrvWalkMoveLeg(evenLegs[i],stepLegsTripod[3][0], walkingDelay * 3U ) << (i*2U));
					success |= (SrvWalkMoveLeg(oddLegs[i],stepLegsTripod[3][1], walkingDelay ) << ((i*2U) + 1U));
				}
				if((success & 0x2AU) == 0x2AU) 
				{
					nextStep = TRUE;
				}
			break;
			
			case 4://{2U,1U},
				for(Int8U i = 0U ; i < 3U ; i++)
				{
					success |= (SrvWalkMoveLeg(evenLegs[i],stepLegsTripod[4][0], walkingDelay * 3U ) << (i*2U));
					success |= (SrvWalkMoveLeg(oddLegs[i],stepLegsTripod[4][1], walkingDelay ) << ((i*2U) + 1U));
				}
				if(success == 0x3FU) 
				{
					nextStep = TRUE;
				}
			break;
			
			case 5://{3U,1U},
				for(Int8U i = 0U ; i < 3U ; i++)
				{
					success |= (SrvWalkMoveLeg(evenLegs[i],stepLegsTripod[5][0], walkingDelay ) << (i*2U));
					success |= (SrvWalkMoveLeg(oddLegs[i],stepLegsTripod[5][1], walkingDelay ) << ((i*2U) + 1U));
				}
				if(success == 0x3FU) 
				{
					sequenceStepTripod = 5;
				}
			break;
			default : 
			break;
		}
	}
	
	if(nextStep == TRUE)
	{
		if( walk == E_WALK_FW )
		{
			sequenceStepTripod == 6U ? sequenceStepTripod = 0U : sequenceStepTripod++;
		}
		if( walk == E_WALK_RV )
		{
			sequenceStepTripod == 255U ? sequenceStepTripod = 5U : sequenceStepTripod--;
		}
	}
}	

static void SrvWalkWave ( E_WALK walk )
{
	Int8U success = FALSE;
	if( walk == E_WALK_FW )
	{
		success |= (SrvWalkMoveLeg(E_LEG_U_L,stepLegsWave[sequenceStepWave][E_LEG_U_L], walkingDelay) << 0);
		success |= (SrvWalkMoveLeg(E_LEG_M_L,stepLegsWave[sequenceStepWave][E_LEG_M_L], walkingDelay) << 1);
		success |= (SrvWalkMoveLeg(E_LEG_B_L,stepLegsWave[sequenceStepWave][E_LEG_B_L], walkingDelay) << 2);
		success |= (SrvWalkMoveLeg(E_LEG_U_R,stepLegsWave[sequenceStepWave][E_LEG_U_R], walkingDelay) << 3);
		success |= (SrvWalkMoveLeg(E_LEG_M_R,stepLegsWave[sequenceStepWave][E_LEG_M_R], walkingDelay) << 4);
		success |= (SrvWalkMoveLeg(E_LEG_B_R,stepLegsWave[sequenceStepWave][E_LEG_B_R], walkingDelay) << 5);
		
		if(success == 0x3FU)
		{
			sequenceStepWave++;
			if(sequenceStepWave == NB_WALK_STEPS_WAVE )
			{
				sequenceStepWave = 0U;
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
	/*if(DrvLegCheckTarget(moveLegs[movingLegIndex]))
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
	}*/
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
		DrvLegSetPosition(E_LEG_U_L, LEG_COCYX_U_L_MIN, LEG_FEMUR_U_L_MAX, LEG_TIBIA_U_L_MAX, 500);
		DrvLegSetPosition(E_LEG_M_L, LEG_COCYX_M_L_MID, LEG_FEMUR_M_L_MAX, LEG_TIBIA_M_L_MAX, 500);
		DrvLegSetPosition(E_LEG_B_L, LEG_COCYX_B_L_MAX, LEG_FEMUR_B_L_MAX, LEG_TIBIA_B_L_MAX, 500);
		DrvLegSetPosition(E_LEG_U_R, LEG_COCYX_U_R_MAX, LEG_FEMUR_U_R_MIN, LEG_TIBIA_U_R_MIN, 500);
		DrvLegSetPosition(E_LEG_M_R, LEG_COCYX_M_R_MID, LEG_FEMUR_M_R_MIN, LEG_TIBIA_M_R_MIN, 500);
		DrvLegSetPosition(E_LEG_B_R, LEG_COCYX_B_R_MIN, LEG_FEMUR_B_R_MIN, LEG_TIBIA_B_R_MIN, 500);
	}
}

static Boolean SrvWalkMoveLeg ( E_LEG movingLeg,Int8U step, Int16U speed )
{
	
	Int8U indexHanche = 0U + (movingLeg * 3U);
	Int8U indexGenoux  = 1U + (movingLeg * 3U);
	Int8U indexCheville = 2U + (movingLeg * 3U);
	
	//set servos position
	uint8_t reponse = 0;
	if(movingLeg < E_LEG_U_R)
	{
		if(movingLeg == E_LEG_U_L)
		{
			reponse = DrvServoSetTarget(indexHanche,	DrvServoGetStruture(indexHanche)->mid + legsXTripodLeft[step] + 20,	speed );
		}
		else if(movingLeg == E_LEG_B_L)
		{
			reponse = DrvServoSetTarget(indexHanche,	DrvServoGetStruture(indexHanche)->mid + legsXTripodLeft[step] - 20,	speed );
		}
		else
		{
			reponse = DrvServoSetTarget(indexHanche,	DrvServoGetStruture(indexHanche)->mid + legsXTripodLeft[step] ,	speed );
		}
		reponse *= 10U;
		reponse += DrvServoSetTarget(indexGenoux,	DrvServoGetStruture(indexGenoux)->mid + legsYTripodLeft[step],	speed );
		reponse *= 10U;
		reponse += DrvServoSetTarget(indexCheville,	DrvServoGetStruture(indexCheville)->mid + legsZTripodLeft[step],	speed );
	}
	else
	{
		if(movingLeg == E_LEG_U_R)
		{
			reponse = DrvServoSetTarget(indexHanche,	DrvServoGetStruture(indexHanche)->mid + legsXTripodRight[step] - 20,	speed );
		}
		else if(movingLeg == E_LEG_B_R)
		{
			reponse = DrvServoSetTarget(indexHanche,	DrvServoGetStruture(indexHanche)->mid + legsXTripodRight[step] + 20,	speed );
		}
		else
		{
			reponse = DrvServoSetTarget(indexHanche,	DrvServoGetStruture(indexHanche)->mid + legsXTripodRight[step] ,	speed );
		}
		reponse *= 10U;
		reponse += DrvServoSetTarget(indexGenoux,	DrvServoGetStruture(indexGenoux)->mid + legsYTripodRight[step],	speed );
		reponse *= 10U;
		reponse += DrvServoSetTarget(indexCheville,	DrvServoGetStruture(indexCheville)->mid + legsZTripodRight[step], speed );
	}
	if(reponse == 111U)
	{
		if(DrvServoCheckPosition(indexHanche) && DrvServoCheckPosition(indexGenoux) && DrvServoCheckPosition(indexCheville))
		{
			return TRUE;
		}
	}
	return FALSE;	
}