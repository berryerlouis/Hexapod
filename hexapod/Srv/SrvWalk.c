////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvTick.h"
#include "SrvWalk.h"
#include "SrvBodyMove.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define NB_WALK_STEPS_TRIANGLE			3
#define NB_WALK_STEPS_SQUARE			4
#define WALK_AMPLITUDE_MAX				40.0
////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

//Stop gaits
static void SrvWalkStep ( void );
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
	{00,  0,	0},				// '\\'
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
	
	//by default go ripple for 1000ms
	walkPrivate.walkStruct.gaiting = E_GAITS_RIPPLE;
	walkPrivate.walkStruct.walking = E_WALK_STOP;
	walkPrivate.walkStruct.delay = 1000U;
	walkPrivate.walkStruct.amplitude = WALK_AMPLITUDE_MAX;
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

SWalk* SrvWalkGetStruct (void) 
{
	return &walkPrivate.walkStruct;
}

Boolean SrvWalkSetSpeed( Int16U speed )
{
	if((speed >= 250) && (speed <= 3000))
	{
		walkPrivate.walkStruct.delay = speed;
		return TRUE;
	}
	return FALSE;
}
Int16U SrvWalkGetSpeed( void )
{
	return walkPrivate.walkStruct.delay;
}

Boolean SrvWalkStopWalk( void )
{
	walkPrivate.walkStruct.walking = E_WALK_STOP;
	return TRUE;
}

Boolean SrvWalkSetWalk( E_GAIT gait, E_WALK walk,Int8S amplitude, Int8S direction, uint16_t delay )
{
	Boolean oSuccess = FALSE;
	
	oSuccess = SrvWalkSetAmplitude(amplitude);
	walkPrivate.walkStruct.direction = direction;
	
	//if we want to change walk
	if((walkPrivate.walkStruct.walking != walk) || (walkPrivate.walkStruct.gaiting != gait))
	{
		walkPrivate.walkStruct.walking = walk;
		walkPrivate.walkStruct.delay = delay;
				
		//if different gait
		if(walkPrivate.walkStruct.gaiting != gait)
		{
			walkPrivate.walkStruct.gaiting = gait;
			//for tripod gait
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
			//for wave gait
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
			//for ripple gait
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
		}
		oSuccess = TRUE;
	}
	else
	{
		//same walk but change speed
		walkPrivate.walkStruct.delay = delay;
		oSuccess = TRUE;
	}
	return oSuccess;
}

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

static volatile float xTemp;
static volatile float yTemp;
static volatile float zTemp;
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

		walkPrivate.walkSteps.indexStep++;
		if( walkPrivate.walkSteps.indexStep == walkPrivate.walkSteps.nbStep)
		{
			walkPrivate.walkSteps.indexStep = 0U;
		}
		
		SBodyMove *move = SrvBodyMoveGetStruct();
		//apply the new angle to legs
		for(ELeg legIndex = E_LEG_F_L ; legIndex < E_NB_LEGS ; legIndex++)
		{
			Int8U stepIndex = 0U;
			Int16U delay = walkPrivate.walkStruct.delay;
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
				stepIndex = 1;
			}
			else if(walkPrivate.walkSteps.steps[legIndex][walkPrivate.walkSteps.indexStep] == '/')
			{
				stepIndex = 2;
			}
			
			//turn over itself
			if((walkPrivate.walkStruct.amplitude == 0) && (walkPrivate.walkStruct.direction != 0))
			{
				//rotate
				//apply direction
				//do clockwise rotation matrix
				xTemp = stepLegsTypeTriangle[ stepIndex ].x;
				yTemp = stepLegsTypeTriangle[ stepIndex ].y;
				
				if(walkPrivate.walkStruct.direction > 0)
				{
					if(legIndex >= E_LEG_F_R)
					{
						xTemp *= -1;
					}
				}
				else
				{
					if(legIndex >= E_LEG_F_R)
					{
						xTemp *= -1;
					}
				}
				
				
				//apply amplitude
				xTemp = xTemp * (float)((WALK_AMPLITUDE_MAX * walkPrivate.walkStruct.direction) / 90);
				yTemp = (yTemp * (float)((((abs(walkPrivate.walkStruct.direction) > WALK_AMPLITUDE_MAX) ? WALK_AMPLITUDE_MAX : walkPrivate.walkStruct.direction) * 20) / WALK_AMPLITUDE_MAX)) + move->groundSize;
				zTemp = stepLegsTypeTriangle[ stepIndex ].z != 0 ? move->elevation  : move->elevation - 20;
			}
			else
			{
				//translate
				//apply direction
				if(legIndex < E_LEG_F_R)
				{
					//do clockwise rotation matrix
					xTemp = stepLegsTypeTriangle[ stepIndex ].x * cos(ToRad(walkPrivate.walkStruct.direction)) + stepLegsTypeTriangle[ stepIndex ].y * sin(ToRad(walkPrivate.walkStruct.direction));
					yTemp = stepLegsTypeTriangle[ stepIndex ].y * cos(ToRad(walkPrivate.walkStruct.direction)) - stepLegsTypeTriangle[ stepIndex ].x * sin(ToRad(walkPrivate.walkStruct.direction));
				}
				else
				{
					//do anticlockwise rotation matrix
					xTemp = (stepLegsTypeTriangle[ stepIndex ].x * cos(ToRad(walkPrivate.walkStruct.direction)) - stepLegsTypeTriangle[ stepIndex ].y * sin(ToRad(walkPrivate.walkStruct.direction)));
					yTemp = (stepLegsTypeTriangle[ stepIndex ].y * cos(ToRad(walkPrivate.walkStruct.direction)) + stepLegsTypeTriangle[ stepIndex ].x * sin(ToRad(walkPrivate.walkStruct.direction)));
				}
				//apply amplitude
				xTemp = xTemp * (float)(walkPrivate.walkStruct.amplitude);
				yTemp = (yTemp * (float)((abs(walkPrivate.walkStruct.amplitude) * 20) / WALK_AMPLITUDE_MAX)) + move->groundSize;
				zTemp = stepLegsTypeTriangle[ stepIndex ].z != 0 ? move->elevation  : move->elevation - 20;
			}	
			
			
			//apply elevation
			
			xTemp = SrvWalkGetLegCoxaAngle(legIndex, xTemp);
			
			
			//SBodyLegMove compute = SrvBodyMoveComputeLegRotationAndTranslation(legIndex);
			
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
		return pos + 60;
	}
	if(legIndex == E_LEG_M_L)
	{
		return pos;
	}
	if(legIndex == E_LEG_R_L)
	{
		return pos - 60;
	}
	if(legIndex == E_LEG_F_R)
	{
		return -pos - 60;
	}
	if(legIndex == E_LEG_M_R)
	{
		return -pos;
	}
	if(legIndex == E_LEG_R_R)
	{
		return -pos + 60;
	}
	return 0;
}



Boolean SrvWalkSetAmplitude( Int8S amplitude )
{
	if(abs(amplitude) <= WALK_AMPLITUDE_MAX )
	{
		walkPrivate.walkStruct.amplitude = amplitude;
		return TRUE;
	}
	return FALSE;
}
Int8S SrvWalkGetAmplitude( void )
{
	return walkPrivate.walkStruct.amplitude;
}
Boolean SrvWalkSetDirection( Int8S direction )
{
	if( abs(direction) <= 90 )
	{
		walkPrivate.walkStruct.direction = direction;
		return TRUE;
	}
	return FALSE;
}
Int8S SrvWalkGetDirection( void )
{
	return walkPrivate.walkStruct.direction;
}