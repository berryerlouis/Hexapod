////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvTick.h"
#include "Drv/DrvLeg.h"
#include "SrvWalk.h"
#include "SrvBodyMove.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define NB_WALK_STEPS_TRIANGLE			3
#define NB_WALK_STEPS_SQUARE			4
#define WALK_AMPLITUDE_MAX			30.0
////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

//Stop gaits
static void SrvWalkStep ( void );
static Boolean SrvWalkSetAmplitude( Int8U amplitude );
static float SrvWalkGetLegCoxaAngle ( ELeg legIndex, float pos );
////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////



typedef struct
{
	float x;
	float y;
	float z;
}SStepNormalPosition;

static SStepNormalPosition stepLegsTypeTriangle[NB_WALK_STEPS_TRIANGLE] =
{
	//x		y	z
	{-1,  0,	1},//ground		// '_'
	{00,  1,	0},				// '\\'
	{01,  0,	1},				// '/'
};


static char tripodSteps[E_NB_LEGS][4] = 
{
	{'/','_','_','\\'},
	{'_','\\','/','_'},
	{'/','_','_','\\'},
	{'_','\\','/','_'},
	{'/','_','_','\\'},
	{'_','\\','/','_'},
};

static char waveSteps[E_NB_LEGS][12] = 
{
	{'/','_','_','_','_','_','_','_','_','_','_','\\'},
	{'_','\\','/','_','_','_','_','_','_','_','_','_'},
	{'_','_','_','\\','/','_','_','_','_','_','_','_'},
	{'_','_','_','_','_','\\','/','_','_','_','_','_'},
	{'_','_','_','_','_','_','_','\\','/','_','_','_'},
	{'_','_','_','_','_','_','_','_','_','\\','/','_'},
};

static char rippleSteps[E_NB_LEGS][8] = 
{
	{'/','_','_','_','_','_','_','\\'},
	{'_','\\','/','_','_','_','_','_'},
	{'_','_','_','\\','/','_','_','_'},
	{'_','_','_','\\','/','_','_','_'},
	{'_','_','_','_','_','\\','/','_'},
	{'/','_','_','_','_','_','_','\\'},
};

//struct of legs,
typedef struct
{
	Int8U *stepLegType;
	char *steps[E_NB_LEGS];
	Int8U indexStep;
	Int8U nbStep;
	Boolean legOnGround[E_NB_LEGS];
	Int16U stepMultDelayOnGround;
}SWalkStep;

//struct of legs,
typedef struct
{
	SWalk walkStruct;
	SWalkStep walkSteps;
}SWalkPrivate;

SWalkPrivate walkPrivate;
////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////

//Fonction d'initialisation
//return : TRUE si ok
Boolean SrvWalkInit ( void )
{
    Boolean oSuccess = TRUE;
	DrvLegInit();
	
	//by default go ripple for 1000ms
	walkPrivate.walkStruct.gaiting = E_GAITS_RIPPLE;
	walkPrivate.walkStruct.walking = E_WALK_STOP;
	walkPrivate.walkStruct.walkingDelay = 1000U;
	walkPrivate.walkStruct.walkingAmplitude = WALK_AMPLITUDE_MAX;
	walkPrivate.walkStruct.direction = 0;
	
	//by default go ripple for 1000ms
	for(ELeg legIndex = E_LEG_F_L ; legIndex < E_NB_LEGS ; legIndex++)
	{
		walkPrivate.walkSteps.steps[legIndex] = rippleSteps[legIndex];
		walkPrivate.walkSteps.legOnGround[legIndex] = FALSE;
	}
	walkPrivate.walkSteps.nbStep = 4U;
	walkPrivate.walkSteps.indexStep = 0U;
	walkPrivate.walkSteps.stepMultDelayOnGround = 12U;

    return oSuccess;
}

//Fonction de dispatching d'evenements
void SrvWalkUpdate ( void )
{
	//only when legs are inittialized
	if(DrvLegIsInitialized())
	{
		//only if different STOP
		if(walkPrivate.walkStruct.walking !=  E_WALK_STOP )
		{
			//update walk step
			SrvWalkStep();
		}
	}	
}

SWalk* SrvWalkgetStruct (void) 
{
	return &walkPrivate.walkStruct;
}

Boolean SrvWalkStopWalk( void )
{
	walkPrivate.walkStruct.walking = E_WALK_STOP;
	return TRUE;
}

Boolean SrvWalkSetWalk( E_GAIT gait, E_WALK walk,Int8U amplitude, Int8S direction, uint16_t delay )
{
	Boolean oSuccess = FALSE;
	
	oSuccess = SrvWalkSetAmplitude(amplitude);
	walkPrivate.walkStruct.direction = direction / 10;
	
	//if we want to change walk
	if((walkPrivate.walkStruct.walking != walk) || (walkPrivate.walkStruct.gaiting != gait))
	{
		walkPrivate.walkStruct.gaiting = gait;
		walkPrivate.walkStruct.walking = walk;
		walkPrivate.walkStruct.walkingDelay = delay;
				
		if( walkPrivate.walkStruct.gaiting == E_GAITS_TRIPOD )
		{
			for(ELeg legIndex = E_LEG_F_L ; legIndex < E_NB_LEGS ; legIndex++)
			{
				walkPrivate.walkSteps.steps[legIndex] = tripodSteps[legIndex];
				walkPrivate.walkSteps.legOnGround[legIndex] = FALSE;
			}
			walkPrivate.walkSteps.nbStep = 4U;
			walkPrivate.walkSteps.indexStep = 0U;
			walkPrivate.walkSteps.stepMultDelayOnGround = 2U;
		}
		else if( walkPrivate.walkStruct.gaiting == E_GAITS_WAVE )
		{
			for(ELeg legIndex = E_LEG_F_L ; legIndex < E_NB_LEGS ; legIndex++)
			{
				walkPrivate.walkSteps.steps[legIndex] = waveSteps[legIndex];
				walkPrivate.walkSteps.legOnGround[legIndex] = FALSE;
			}
			walkPrivate.walkSteps.nbStep = 12U;
			walkPrivate.walkSteps.indexStep = 0U;
			walkPrivate.walkSteps.stepMultDelayOnGround = 10U;
		}
		else if( walkPrivate.walkStruct.gaiting == E_GAITS_RIPPLE )
		{
			for(ELeg legIndex = E_LEG_F_L ; legIndex < E_NB_LEGS ; legIndex++)
			{
				walkPrivate.walkSteps.steps[legIndex] = rippleSteps[legIndex];
				walkPrivate.walkSteps.legOnGround[legIndex] = FALSE;
			}
			walkPrivate.walkSteps.nbStep = 8U;
			walkPrivate.walkSteps.indexStep = 0U;
			walkPrivate.walkSteps.stepMultDelayOnGround = 6U;
		}
		
		oSuccess = TRUE;
	}
	else
	{
		//same walk but change speed
		walkPrivate.walkStruct.walkingDelay = delay;
		oSuccess = TRUE;
	}
	return oSuccess;
}

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

static void SrvWalkStep ( void )
{
	//check all legs to reach their target
	Int8U okTarget = 0U;
	for(ELeg legIndex = E_LEG_F_L ; legIndex < E_NB_LEGS ; legIndex++)
	{
		//check leg position
		if( DrvLegCheckTarget(legIndex) )
		{
			//leg reach position and could go to another position
			okTarget += 1U << legIndex;
		}
		else
		{
			//if leg is on ground seems to ok for next step
			if( walkPrivate.walkSteps.legOnGround[legIndex] == TRUE )
			{
				okTarget += 1U << legIndex;
			}
		}
	}
	
	//all legs reach their target, so we can go further
	if(okTarget == 0b00111111)
	{
		//go to the next step
		if(walkPrivate.walkStruct.walking == E_WALK_FW)
		{
			walkPrivate.walkSteps.indexStep++;
			if( walkPrivate.walkSteps.indexStep == walkPrivate.walkSteps.nbStep)
			{
				walkPrivate.walkSteps.indexStep = 0U;
			}
		}
		else if(walkPrivate.walkStruct.walking == E_WALK_RV)
		{
			walkPrivate.walkSteps.indexStep--;
			if( walkPrivate.walkSteps.indexStep == 0xFF)
			{
				walkPrivate.walkSteps.indexStep = walkPrivate.walkSteps.nbStep - 1U;
			}
		}
		
		SBodyMove *move = SrvBodyMoveGetStruct();
		//apply the new angle to legs
		for(ELeg legIndex = E_LEG_F_L ; legIndex < E_NB_LEGS ; legIndex++)
		{
			float xTemp;
			float yTemp;
			float zTemp;
			Int8U stepIndex = 0U;
			Int16U delay = walkPrivate.walkStruct.walkingDelay;
			walkPrivate.walkSteps.legOnGround[legIndex] = FALSE;
			
			//go forward
			if(walkPrivate.walkSteps.steps[legIndex][walkPrivate.walkSteps.indexStep] == '_')
			{
				stepIndex = 0;
				walkPrivate.walkSteps.legOnGround[legIndex] = TRUE;
				delay *= walkPrivate.walkSteps.stepMultDelayOnGround;
			}
			else if(walkPrivate.walkSteps.steps[legIndex][walkPrivate.walkSteps.indexStep] == '\\')
			{
				//swap index if reverse direction
				stepIndex = (walkPrivate.walkStruct.walking == E_WALK_FW) ? 1 : 2;
			}
			else if(walkPrivate.walkSteps.steps[legIndex][walkPrivate.walkSteps.indexStep] == '/')
			{
				//swap index if reverse direction
				stepIndex = (walkPrivate.walkStruct.walking == E_WALK_FW) ? 2 : 1;
			}
			
			//get position at index		
			xTemp = (walkPrivate.walkStruct.walking == E_WALK_FW) ? stepLegsTypeTriangle[ stepIndex ].x : -stepLegsTypeTriangle[ stepIndex ].x;
			//apply amplitude
			xTemp = (xTemp + walkPrivate.walkStruct.direction) * (float)(walkPrivate.walkStruct.walkingAmplitude);
			//apply direction
			/*if(legIndex < E_LEG_F_R)
			{
				xTemp = xTemp * (float)(walkPrivate.walkStruct.walkingAmplitude + walkPrivate.walkStruct.direction);
			}
			else
			{
				xTemp = xTemp * (float)(walkPrivate.walkStruct.walkingAmplitude + walkPrivate.walkStruct.direction);
			}*/
			
			xTemp = SetLimits(xTemp,-WALK_AMPLITUDE_MAX,WALK_AMPLITUDE_MAX);
			xTemp = SrvWalkGetLegCoxaAngle(legIndex, xTemp);
			yTemp = stepLegsTypeTriangle[ stepIndex ].y == 0 ? move->groundSize : move->groundSize + ((walkPrivate.walkStruct.walkingAmplitude * 20) / WALK_AMPLITUDE_MAX); //between ground size and ground size + 20 (relative to amplitude)
			zTemp = stepLegsTypeTriangle[ stepIndex ].z != 0 ? move->elevation  : move->elevation - ((walkPrivate.walkStruct.walkingAmplitude * 10) / WALK_AMPLITUDE_MAX) ; //between elevation and 0
			
			DrvLegSetTarget(legIndex,
							xTemp,
							yTemp,
							zTemp,
							delay);
		}
	}
	
}



static float SrvWalkGetLegCoxaAngle ( ELeg legIndex, float pos )
{
	if(legIndex == E_LEG_F_L)
	{
		return pos + WALK_AMPLITUDE_MAX;
	}
	if(legIndex == E_LEG_M_L)
	{
		return pos;
	}
	if(legIndex == E_LEG_R_L)
	{
		return pos - WALK_AMPLITUDE_MAX;
	}
	if(legIndex == E_LEG_F_R)
	{
		return -pos - WALK_AMPLITUDE_MAX;
	}
	if(legIndex == E_LEG_M_R)
	{
		return -pos;
	}
	if(legIndex == E_LEG_R_R)
	{
		return -pos + WALK_AMPLITUDE_MAX;
	}
	return 0;
}


static Boolean SrvWalkSetAmplitude( Int8U amplitude )
{
	if( amplitude <= WALK_AMPLITUDE_MAX )
	{
		walkPrivate.walkStruct.walkingAmplitude = amplitude;
		return TRUE;
	}
	return FALSE;
}
