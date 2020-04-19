////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvTick.h"
#include "Drv/DrvLeg.h"
#include "SrvWalk.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define NB_WALK_STEPS_TRIPOD	3U
#define WALK_AMPLITUDE			45
////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

//Stop gaits
static void SrvWalkTripod ( E_WALK walk );
static void SrvWalkWave ( E_WALK walk );
static void SrvWalkRipple ( E_WALK walk );
////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////



typedef struct
{
	float x;
	float y;
	float z;
}SStep;

static SStep stepLegsTripod[NB_WALK_STEPS_TRIPOD] =
{
	//x		y	z 
	{00, 100,	00},
	{WALK_AMPLITUDE, 90,	50},
	{-WALK_AMPLITUDE, 90,	50},//ground
};
static SStep stepLegsTripodRV[NB_WALK_STEPS_TRIPOD] =
{
	//x		y	z
	{00, 100,	00},
	{-WALK_AMPLITUDE, 90,	50},//ground
	{WALK_AMPLITUDE, 90,	50},
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
	walkStruct.gaiting = E_GAITS_TRIPOD;
	walkStruct.walking = E_WALK_PAUSE;
	walkStruct.walkingDelay = 1000U;
	
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

Boolean SrvWalkSetWalk( E_WALK walk, uint16_t delay )
{
    Boolean oSuccess = FALSE;
	if( walkStruct.walking ==  E_WALK_STOP )
	{
		legSteps[E_LEG_U_L] = 0U;
		legSteps[E_LEG_M_L] = 0U;
		legSteps[E_LEG_B_L] = 0U;
		legSteps[E_LEG_U_R] = 0U;
		legSteps[E_LEG_M_R] = 0U;
		legSteps[E_LEG_B_R] = 0U;
	}
	
	if(walkStruct.walking != walk)
	{
		if(walk == E_WALK_FW)
		{
			reverse = FALSE;
		}
		if(walk == E_WALK_RV)
		{
			reverse = TRUE;
		}
		walkStruct.walking = walk;
		walkStruct.walkingDelay = delay;
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
		legSteps[E_LEG_U_L] = 0U;
		legSteps[E_LEG_M_L] = 0U;
		legSteps[E_LEG_B_L] = 0U;
		legSteps[E_LEG_U_R] = 0U;
		legSteps[E_LEG_M_R] = 0U;
		legSteps[E_LEG_B_R] = 0U;
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
	for(Int8U legIndex = 0 ; legIndex < E_NB_LEGS ; legIndex++)
	{
		//if needed to compute new angle
		if( DrvLegCheckTarget(legIndex) )
		{
			Int8U stepIndex = legSteps[legIndex] % 3;
			if((legIndex == E_LEG_M_L) || (legIndex == E_LEG_U_R) || (legIndex == E_LEG_B_R))
			{
				stepIndex = (legSteps[legIndex] + 2) % 3;
			}
			
			//delay multiply by 2 when leg is on ground
			Int16U delay = walkStruct.walkingDelay;
			if(stepIndex == 2) delay*=2U;
			
			float xTemp;
			float yTemp;
			float zTemp;
			if(reverse == FALSE)
			{
				xTemp = stepLegsTripod[ stepIndex ].x;
				yTemp = stepLegsTripod[ stepIndex ].y;
				zTemp = stepLegsTripod[ stepIndex ].z;
			}
			else
			{
				xTemp = stepLegsTripodRV[ stepIndex ].x;
				yTemp = stepLegsTripodRV[ stepIndex ].y;
				zTemp = stepLegsTripodRV[ stepIndex ].z;
			}
			if(legIndex == E_LEG_U_L)
			{
				xTemp = xTemp + WALK_AMPLITUDE;
			}
			if(legIndex == E_LEG_M_L)
			{
				xTemp = xTemp;
			}
			if(legIndex == E_LEG_B_L) 
			{
				xTemp = xTemp - WALK_AMPLITUDE;
			}
			if(legIndex == E_LEG_U_R)
			{
				xTemp = -xTemp - WALK_AMPLITUDE;
			}
			if(legIndex == E_LEG_M_R)
			{
				xTemp = -xTemp;
			}
			if(legIndex == E_LEG_B_R)
			{
				xTemp = -xTemp + WALK_AMPLITUDE;
			}
			
			DrvLegSetTarget(legIndex,
							xTemp,
							yTemp,
							zTemp,
							delay);
			//next step
			legSteps[legIndex]++;
			//reset
			if( legSteps[legIndex] == NB_WALK_STEPS_TRIPOD)
			{
				legSteps[legIndex] = 0U;
			}
		}
	}
}	

static void SrvWalkWave ( E_WALK walk )
{	
	//if needed to compute new angle
	if( DrvLegCheckTarget(legIndexWave) )
	{
		Int8U stepIndex = legSteps[legIndexWave] % 3;
		
		//delay multiply by 2 when leg is on ground
		Int16U delay = walkStruct.walkingDelay;
		if(stepIndex == 2) delay*=2U;
		
		float xTemp;
		float yTemp;
		float zTemp;
		if(reverse == FALSE)
		{
			xTemp = stepLegsTripod[ stepIndex ].x;
			yTemp = stepLegsTripod[ stepIndex ].y;
			zTemp = stepLegsTripod[ stepIndex ].z;
		}
		else
		{
			xTemp = stepLegsTripodRV[ stepIndex ].x;
			yTemp = stepLegsTripodRV[ stepIndex ].y;
			zTemp = stepLegsTripodRV[ stepIndex ].z;
		}
		if(legIndexWave == E_LEG_U_L)
		{
			xTemp = xTemp + WALK_AMPLITUDE;
		}
		if(legIndexWave == E_LEG_M_L)
		{
			xTemp = xTemp;
		}
		if(legIndexWave == E_LEG_B_L)
		{
			xTemp = xTemp - WALK_AMPLITUDE;
		}
		if(legIndexWave == E_LEG_U_R)
		{
			xTemp = -xTemp - WALK_AMPLITUDE;
		}
		if(legIndexWave == E_LEG_M_R)
		{
			xTemp = -xTemp;
		}
		if(legIndexWave == E_LEG_B_R)
		{
			xTemp = -xTemp + WALK_AMPLITUDE;
		}
		
		DrvLegSetTarget(legIndexWave,
		xTemp,
		yTemp,
		zTemp,
		delay);
		//next step
		legSteps[legIndexWave]++;
		//reset
		if( legSteps[legIndexWave] == NB_WALK_STEPS_TRIPOD)
		{
			legSteps[legIndexWave] = 0U;
			legIndexWave++;
			if( legIndexWave == E_NB_LEGS)
			{
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
		Int8U stepIndex = legSteps[legRipple[legIndexRipple]] % 3;
		//delay multiply by 2 when leg is on ground
		Int16U delay = walkStruct.walkingDelay;
		if(stepIndex == 2) delay*=2U;
		
		float xTemp;
		float yTemp;
		float zTemp;
		if(reverse == FALSE)
		{
			xTemp = stepLegsTripod[ stepIndex ].x;
			yTemp = stepLegsTripod[ stepIndex ].y;
			zTemp = stepLegsTripod[ stepIndex ].z;
		}
		else
		{
			xTemp = stepLegsTripodRV[ stepIndex ].x;
			yTemp = stepLegsTripodRV[ stepIndex ].y;
			zTemp = stepLegsTripodRV[ stepIndex ].z;
		}
		if(legRipple[legIndexRipple] == E_LEG_U_L)
		{
			xTemp = xTemp + WALK_AMPLITUDE;
		}
		if(legRipple[legIndexRipple] == E_LEG_M_L)
		{
			xTemp = xTemp;
		}
		if(legRipple[legIndexRipple] == E_LEG_B_L)
		{
			xTemp = xTemp - WALK_AMPLITUDE;
		}
		if(legRipple[legIndexRipple] == E_LEG_U_R)
		{
			xTemp = -xTemp - WALK_AMPLITUDE;
		}
		if(legRipple[legIndexRipple] == E_LEG_M_R)
		{
			xTemp = -xTemp;
		}
		if(legRipple[legIndexRipple] == E_LEG_B_R)
		{
			xTemp = -xTemp + WALK_AMPLITUDE;
		}
		
		DrvLegSetTarget(legRipple[legIndexRipple],
		xTemp,
		yTemp,
		zTemp,
		delay);
		//next step
		legSteps[legRipple[legIndexRipple]]++;
		//reset
		if( legSteps[legRipple[legIndexRipple]] == NB_WALK_STEPS_TRIPOD)
		{
			legSteps[legRipple[legIndexRipple]] = 0U;
			
			legIndexRipple++;
			if( legIndexRipple == E_NB_LEGS)
			{
				legIndexRipple = 0U;
			}
		}
	}
}









