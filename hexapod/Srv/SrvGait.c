////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvLeg.h"
#include "Drv/DrvTick.h"

#include "SrvGait.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

#define BODY_SIZE_Y	150
#define BODY_SIZE_X	80
#define BODY_SIZE_MX	130

#define LEG_POS_IDLE_X	-LEG_COCYX_LENGTH
#define LEG_POS_IDLE_Y	0
#define LEG_POS_IDLE_Z	LEG_TIBIA_LENGTH

#define BODY_SIZE_OFFSET_X	(BODY_SIZE_X / 2)
#define BODY_SIZE_OFFSET_MX	(BODY_SIZE_MX / 2)
#define BODY_SIZE_OFFSET_Y	(BODY_SIZE_Y / 2)
#define BODY_SIZE_OFFSET_Z	0

#define BODY_LEG_FL_POS_X	-BODY_SIZE_OFFSET_X
#define BODY_LEG_FL_POS_Y	BODY_SIZE_OFFSET_Y
#define BODY_LEG_FL_POS_Z	BODY_SIZE_OFFSET_Z

#define LEG_FL_POS_X	LEG_POS_IDLE_X
#define LEG_FL_POS_Y	LEG_POS_IDLE_Y
#define LEG_FL_POS_Z	LEG_POS_IDLE_Z

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
static AxisFloat SrvGaitComputeInverseKinematics(ELeg indexLeg, AxisFloat positionBot, AxisFloat rotationBot);

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
Int32U prevMillisGait = 0U;
AxisFloat positionBot[3U];

AxisFloat legPosition [E_NB_LEGS];
////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////

static Int8U toto = 0;
static Int32U titi = 0;

static volatile float BodyCenterOffset1;
static volatile float BodyCenterOffset2;
static volatile float BodyCenterOffset3;
static volatile float BodyCenterOffsetX[E_NB_LEGS];
static volatile float BodyCenterOffsetY[E_NB_LEGS];


//Fonction d'initialisation
Boolean SrvGaitInit ( void ) 
{
	positionBot[0].x = 0;
	positionBot[0].y = 65;
	positionBot[0].z = 0;
	positionBot[1].x = 45;
	positionBot[1].y = 65;
	positionBot[1].z = -20;
	positionBot[2].x = 60;
	positionBot[2].y = 65;
	positionBot[2].z = 0;
	
	//position leg from center body
	BodyCenterOffset1 = BODY_SIZE_X/2;
	BodyCenterOffset2 = BODY_SIZE_Y/2;
	BodyCenterOffset3 = BODY_SIZE_MX/2;
			
	//distance from body center to leg
	BodyCenterOffsetX[E_LEG_F_L] = -BodyCenterOffset1; 
	BodyCenterOffsetY[E_LEG_F_L] = BodyCenterOffset2; 
	BodyCenterOffsetX[E_LEG_M_L] = -BodyCenterOffset3; 
	BodyCenterOffsetY[E_LEG_M_L] = 0; 
	
	return DrvLegInit();
}


//Fonction de dispatching d'evenements
void SrvGaitUpdate (void)
{
	DrvLegUpdate();
	if ((DrvTickGetTimeMs() - prevMillisGait) > 2500U)
	{	
		prevMillisGait = DrvTickGetTimeMs();
		DrvLegSetTarget(E_LEG_F_L,positionBot[toto],1000U);
		DrvLegSetTarget(E_LEG_M_L,positionBot[toto],1000U);
		DrvLegSetTarget(E_LEG_R_L,positionBot[toto],1000U);
		toto++;
		if(toto == 3) 
		{
			toto = 0;
		}
		//for(ELeg indexLeg = 0U ; indexLeg < E_NB_LEGS ; indexLeg++ )
		{
			//if(DrvLegReachPosition(indexLeg))
			{
				/*AxisFloat position = SrvGaitComputeInverseKinematics(indexLeg, positionBot[toto], rotationBot[toto]);
				DrvLegSetTarget(indexLeg,position,1000U);
				titi |= indexLeg;
				if(titi == 0b111)
				{
					titi=0;
					toto++;
					if(toto == 3) 
					{
						toto = 0;
					}
				}*/
			}
		}
	}
}
