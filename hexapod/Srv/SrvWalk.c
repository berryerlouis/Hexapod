////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvTick.h"
#include "Drv/DrvLeg.h"
#include "SrvWalk.h"
#include "SrvBodyMove.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define NB_WALK_STEPS			3
#define WALK_AMPLITUDE			30.0
////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

//Stop gaits
static void SrvWalkTripod ( E_WALK walk );
static void SrvWalkWave ( E_WALK walk );
static void SrvWalkRipple ( E_WALK walk );

static float SrvWalkGetLegCoxaAngle ( ELeg legIndex, float pos );
////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////



typedef struct
{
	float x;
	float y;
	float z;
}SStep;

static SStep stepLegsTripod[NB_WALK_STEPS] =
{
	//x		y	z
	{-1,  0,	1},//ground
	{00,  1,	0},
	{01,  0,	1},
};
static SStep stepLegsTripodRV[NB_WALK_STEPS] =
{
	//x		y	z
	{01,  0,	1},
	{00,  1,	0},
	{-1,  0,	1},//ground
};

static Int8S legSteps [NB_LEGS] ;	
static Boolean reverse = FALSE;
static ELeg legIndexWave = E_LEG_U_L;
static Int8U legIndexRipple = 0;
static ELeg legRipple[] = {E_LEG_U_L,E_LEG_M_R,E_LEG_B_L,E_LEG_U_R,E_LEG_M_L,E_LEG_B_R};

SWalk walkStruct;

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////

//Fonction d'initialisation
//return : TRUE si ok
Boolean SrvWalkInit ( void )
{
    Boolean oSuccess = TRUE;
	DrvLegInit();
	walkStruct.gaiting = E_GAITS_RIPPLE;
	walkStruct.walking = E_WALK_PAUSE;
	walkStruct.walkingDelay = 1000U;
	walkStruct.walkingAmplitude = WALK_AMPLITUDE;
	SrvWalkSetWalk(walkStruct.walking, walkStruct.walkingDelay);
	SrvWalkSetGait(walkStruct.gaiting, walkStruct.walkingDelay);

    return oSuccess;
}

//Fonction de dispatching d'evenements
void SrvWalkUpdate ( void )
{
	//only when legs are inittialized
	if(DrvLegIsInitialized())
	{
		//only if different STOP
		if(( walkStruct.walking !=  E_WALK_PAUSE ) && ( walkStruct.walking !=  E_WALK_STOP ))
		{
			//update walk step
			if( walkStruct.gaiting == E_GAITS_TRIPOD )
			{
				SrvWalkTripod(walkStruct.gaiting);
			}
			else if( walkStruct.gaiting == E_GAITS_WAVE )
			{
				SrvWalkWave(walkStruct.gaiting);
			}
			else if( walkStruct.gaiting == E_GAITS_RIPPLE )
			{
				SrvWalkRipple(walkStruct.gaiting);
			}
		}
	}	
}

SWalk* SrvWalkgetStruct (void) 
{
	return &walkStruct;
}

Boolean SrvWalkSetAmplitude( Int8U amplitude )
{
	if( amplitude <= WALK_AMPLITUDE )
	{
		walkStruct.walkingAmplitude = amplitude;
		return TRUE;
	}
	return FALSE;
}
Boolean SrvWalkSetWalk( E_WALK walk, uint16_t delay )
{
    Boolean oSuccess = FALSE;
	
	if(walkStruct.walking != walk)
	{
		walkStruct.walking = walk;
		walkStruct.walkingDelay = delay;
		if((walk == E_WALK_PAUSE) || (walk == E_WALK_STOP))
		{
			SrvBodyMoveSetBehavior(E_BODY_STAR, walkStruct.walkingDelay);
		}		
		else
		{
			if(walk == E_WALK_FW)
			{
				reverse = FALSE;
			}
			if(walk == E_WALK_RV)
			{
				reverse = TRUE;
			}
			legIndexWave = E_LEG_U_L;
			legIndexRipple = 0U;
			if( walkStruct.gaiting == E_GAITS_TRIPOD )
			{
				legSteps[E_LEG_U_L] = 1U;
				legSteps[E_LEG_M_L] = 2U;
				legSteps[E_LEG_B_L] = 1U;
				legSteps[E_LEG_U_R] = 2U;
				legSteps[E_LEG_M_R] = 1U;
				legSteps[E_LEG_B_R] = 2U;
			}
			else if( walkStruct.gaiting == E_GAITS_WAVE )
			{
				legSteps[E_LEG_U_L] = 1U;
				legSteps[E_LEG_M_L] = 1U;
				legSteps[E_LEG_B_L] = 1U;
				legSteps[E_LEG_U_R] = 1U;
				legSteps[E_LEG_M_R] = 1U;
				legSteps[E_LEG_B_R] = 1U;
			}
			else if( walkStruct.gaiting == E_GAITS_RIPPLE )
			{
				legSteps[E_LEG_U_L] = 0U;
				legSteps[E_LEG_M_L] = 0U;
				legSteps[E_LEG_B_L] = 0U;
				legSteps[E_LEG_U_R] = 0U;
				legSteps[E_LEG_M_R] = 0U;
				legSteps[E_LEG_B_R] = 0U;
			}
		}
		
		oSuccess = TRUE;
	}
	else
	{
		walkStruct.walkingDelay = delay;
		oSuccess = TRUE;
	}
    return oSuccess;
}
Boolean SrvWalkSetGait( E_GAIT gait, uint16_t delay )
{
	Boolean oSuccess = FALSE;
	
	if(walkStruct.gaiting != gait)
	{
		walkStruct.gaiting = gait;
		walkStruct.walkingDelay = delay;	
		legIndexWave = E_LEG_U_L;	
		legIndexRipple = 0;
		oSuccess = TRUE;
	}
	else
	{
		walkStruct.walkingDelay = delay;
		oSuccess = TRUE;
	}
	return oSuccess;
}

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
static void SrvWalkTripod ( E_WALK walk )
{	
	//update for each leg
	for(ELeg legIndex = 0 ; legIndex < E_NB_LEGS ; legIndex++)
	{
		//if needed to compute new angle
		if( DrvLegCheckTarget(legIndex) )
		{
			Int8U stepIndex = legSteps[legIndex] % 3;
			if((legIndex == E_LEG_M_L) || (legIndex == E_LEG_U_R) || (legIndex == E_LEG_B_R))
			{
				stepIndex = (legSteps[legIndex] + 1) % 3;
			}
			
			//delay multiply by 2 when leg is on ground
			Int16U delay = walkStruct.walkingDelay;
			if(stepIndex == 0) delay*=2U;
			
			float xTemp;
			float yTemp;
			float zTemp;
			SBodyMove *move = SrvBodyMoveGetStruct();
			if(reverse == FALSE)
			{
				xTemp = SrvWalkGetLegCoxaAngle(legIndex, stepLegsTripod[ stepIndex ].x * walkStruct.walkingAmplitude);
				yTemp = stepLegsTripod[ stepIndex ].y == 0 ? move->groundSize : move->groundSize + 10; //between ground size and round size + 10
				zTemp = stepLegsTripod[ stepIndex ].z != 0 ? move->elevation : ((WALK_AMPLITUDE - walkStruct.walkingAmplitude) / WALK_AMPLITUDE) * move->elevation; //between elevation and 0
			}
			else
			{
				xTemp = SrvWalkGetLegCoxaAngle(legIndex, stepLegsTripodRV[ stepIndex ].x * walkStruct.walkingAmplitude);
				yTemp = stepLegsTripodRV[ stepIndex ].y == 0 ? move->groundSize : move->groundSize + 10; //between ground size and round size + 10
				zTemp = stepLegsTripodRV[ stepIndex ].z != 0 ? move->elevation : ((WALK_AMPLITUDE - walkStruct.walkingAmplitude) / WALK_AMPLITUDE) * move->elevation;
			}
			
			DrvLegSetTarget(legIndex,
							xTemp,
							yTemp,
							zTemp,
							delay);
			//synchronisation of legs
			if( legIndex % 2 == 0 )		
			{
				//even legs
				if(DrvLegCheckTarget(E_LEG_U_L) && DrvLegCheckTarget(E_LEG_B_L) && DrvLegCheckTarget(E_LEG_M_R))
				{
					legSteps[legIndex] = (legSteps[legIndex] + 1U) % 3;
				}
			}
			else
			{
				//odd legs
				if(DrvLegCheckTarget(E_LEG_U_R) && DrvLegCheckTarget(E_LEG_B_R) && DrvLegCheckTarget(E_LEG_M_L))
				{
					legSteps[legIndex] = (legSteps[legIndex] + 1U) % 3;
				}
			}	
			//reset
			if( legSteps[legIndex] == NB_WALK_STEPS)
			{
				legSteps[legIndex] = 0U;
			}
		}
	}
}	

static void SrvWalkWave ( E_WALK walk )
{	
	//all leg do only two step
	//if needed to compute new angle
	if( legIndexWave != E_NB_LEGS && DrvLegCheckTarget(legIndexWave) )
	{ 
		Int8U stepIndex = legSteps[legIndexWave] % 3;
		
		//delay multiply by 2 when leg is on ground
		Int16U delay = walkStruct.walkingDelay;
		
		float xTemp;
		float yTemp;
		float zTemp;
		SBodyMove *move = SrvBodyMoveGetStruct();
		if(reverse == FALSE)
		{
			xTemp = SrvWalkGetLegCoxaAngle(legIndexWave, stepLegsTripod[ stepIndex ].x * walkStruct.walkingAmplitude);
			yTemp = stepLegsTripod[ stepIndex ].y == 0 ? move->groundSize : move->groundSize + 10; //between ground size and round size + 10
			zTemp = stepLegsTripod[ stepIndex ].z != 0 ? move->elevation : ((WALK_AMPLITUDE - walkStruct.walkingAmplitude) /WALK_AMPLITUDE) * move->elevation;
		}
		else
		{
			xTemp = SrvWalkGetLegCoxaAngle(legIndexWave, stepLegsTripodRV[ stepIndex ].x * walkStruct.walkingAmplitude);
			yTemp = stepLegsTripodRV[ stepIndex ].y == 0 ? move->groundSize : move->groundSize + 10; //between ground size and round size + 10
			zTemp = stepLegsTripodRV[ stepIndex ].z != 0 ? move->elevation : ((WALK_AMPLITUDE - walkStruct.walkingAmplitude) /WALK_AMPLITUDE) * move->elevation;
		}
		
		DrvLegSetTarget(legIndexWave,
						xTemp,
						yTemp,
						zTemp,
						delay);
						
		legSteps[legIndexWave] = (legSteps[legIndexWave] + 1U) % 3;
		
		//reset
		if( legSteps[legIndexWave] == NB_WALK_STEPS )
		{
			legSteps[legIndexWave] = 0;
			legIndexWave++;
			if( legIndexWave == E_NB_LEGS)
			{
				legSteps[E_LEG_U_L] = 0;
				legSteps[E_LEG_M_L] = 0;
				legSteps[E_LEG_B_L] = 0;
				legSteps[E_LEG_U_R] = 0;
				legSteps[E_LEG_M_R] = 0;
				legSteps[E_LEG_B_R] = 0;
			}
		}
	}
	//all leg do in same time push
	else if( legIndexWave == E_NB_LEGS )
	{
		//all leg must be ok
		if( (DrvLegCheckTarget(E_LEG_U_L) && DrvLegCheckTarget(E_LEG_B_L) && DrvLegCheckTarget(E_LEG_M_R)) &&
			(DrvLegCheckTarget(E_LEG_U_R) && DrvLegCheckTarget(E_LEG_B_R) && DrvLegCheckTarget(E_LEG_M_L)))
		{
			//update for each leg
			for(ELeg legIndex = 0 ; legIndex < E_NB_LEGS ; legIndex++)
			{
				Int8U stepIndex = 0;
				Int16U delay = walkStruct.walkingDelay;
				
				float xTemp;
				float yTemp;
				float zTemp;
				SBodyMove *move = SrvBodyMoveGetStruct();
				if(reverse == FALSE)
				{
					xTemp = SrvWalkGetLegCoxaAngle(legIndex, stepLegsTripod[ stepIndex ].x * walkStruct.walkingAmplitude);
					yTemp = stepLegsTripod[ stepIndex ].y == 0 ? move->groundSize : move->groundSize + 10; //between ground size and round size + 10
					zTemp = stepLegsTripod[ stepIndex ].z != 0 ? move->elevation : ((WALK_AMPLITUDE - walkStruct.walkingAmplitude) /WALK_AMPLITUDE) * move->elevation;
				}
				else
				{
					xTemp = SrvWalkGetLegCoxaAngle(legIndex, stepLegsTripodRV[ stepIndex ].x * walkStruct.walkingAmplitude);
					yTemp = stepLegsTripodRV[ stepIndex ].y == 0 ? move->groundSize : move->groundSize + 10; //between ground size and round size + 10
					zTemp = stepLegsTripodRV[ stepIndex ].z != 0 ? move->elevation : ((WALK_AMPLITUDE - walkStruct.walkingAmplitude) /WALK_AMPLITUDE) * move->elevation;
				}
				
				DrvLegSetTarget(legIndex,
					xTemp,
					yTemp,
					zTemp,
					delay);
				//next step
				legSteps[legIndex] = 0U;
				legIndexWave = E_LEG_U_L;
			}
		}
	}
}

static void SrvWalkRipple ( E_WALK walk )
{	
	//if needed to compute new angle
	if( DrvLegCheckTarget(legRipple[legIndexRipple]) )
	{
			
		legSteps[legRipple[legIndexRipple]] = (legSteps[legRipple[legIndexRipple]] + 1U) % 3;
		Int8U stepIndex = (legSteps[legRipple[legIndexRipple]] % 3);
		//delay multiply by 2 when leg is on ground
		Int16U delay = walkStruct.walkingDelay;
		if(stepIndex == 0) delay*=10U;
			
		float xTemp;
		float yTemp;
		float zTemp;
		SBodyMove *move = SrvBodyMoveGetStruct();
		if(reverse == FALSE)
		{
			xTemp = SrvWalkGetLegCoxaAngle(legRipple[legIndexRipple], stepLegsTripod[ stepIndex ].x * walkStruct.walkingAmplitude);
			yTemp = stepLegsTripod[ stepIndex ].y == 0 ? move->groundSize : move->groundSize + 10; //between ground size and round size + 10
			zTemp = stepLegsTripod[ stepIndex ].z != 0 ? move->elevation : ((WALK_AMPLITUDE - walkStruct.walkingAmplitude) / WALK_AMPLITUDE) * move->elevation; //between elevation and 0
		}
		else
		{
			xTemp = SrvWalkGetLegCoxaAngle(legRipple[legIndexRipple], stepLegsTripodRV[ stepIndex ].x * walkStruct.walkingAmplitude);
			yTemp = stepLegsTripodRV[ stepIndex ].y == 0 ? move->groundSize : move->groundSize + 10; //between ground size and round size + 10
			zTemp = stepLegsTripodRV[ stepIndex ].z != 0 ? move->elevation : ((WALK_AMPLITUDE - walkStruct.walkingAmplitude) / WALK_AMPLITUDE) * move->elevation;
		}
			
		DrvLegSetTarget(legRipple[legIndexRipple],
						xTemp,
						yTemp,
						zTemp,
						delay);
							
		//reset
		if( (legSteps[legRipple[legIndexRipple]] % 3) == 0)
		{
			legSteps[legRipple[legIndexRipple]]  = 0U;
			legIndexRipple++;
			if( legIndexRipple == E_NB_LEGS)
			{
				legIndexRipple = 0U;
			}
		}
	}
}




static float SrvWalkGetLegCoxaAngle ( ELeg legIndex, float pos )
{
	if(legIndex == E_LEG_U_L)
	{
		return pos + WALK_AMPLITUDE;
	}
	if(legIndex == E_LEG_M_L)
	{
		return pos;
	}
	if(legIndex == E_LEG_B_L)
	{
		return pos - WALK_AMPLITUDE;
	}
	if(legIndex == E_LEG_U_R)
	{
		return -pos - WALK_AMPLITUDE;
	}
	if(legIndex == E_LEG_M_R)
	{
		return -pos;
	}
	if(legIndex == E_LEG_B_R)
	{
		return -pos + WALK_AMPLITUDE;
	}
	return 0;
}




