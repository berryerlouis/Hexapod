/*
 * SrvWalk.h
 *
 * Created: 27/07/2017 16:04:46
 *  Author: berryer
 */ 


#ifndef SRVWALK_H_
#define SRVWALK_H_

#include "../Conf/ConfHard.h"
#include "../Tools/tools_typedefs.h"


////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC ENUM///////////////////////////////////////////////
 
typedef enum
{
	E_GAITS_TRIPOD,
	E_GAITS_WAVE,
	E_GAITS_RIPPLE,
}E_GAIT;

typedef enum
{
	E_WALK_PAUSE,
	E_WALK_FW,
	E_WALK_RV,
	E_WALK_STOP,
	E_WALK_UNKNOWN,
}E_WALK;

#define GAITS_TRIPOD_STEPS	2U
#define GAITS_WAVE_STEPS	6U
#define GAITS_RIPPLE_STEPS	6U

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////

//E_GAITS_TRIPOD
//UL [0][1]
//ML [1][0]
//BL [0][1]
//UR [1][0]
//MR [0][1]
//BR [1][0]

//E_GAITS_WAVE
//UL [0][0][0][0][0][1]
//ML [0][0][0][0][1][0]
//BL [0][0][0][1][0][0]
//UR [0][0][1][0][0][0]
//MR [0][1][0][0][0][0]
//BR [1][0][0][0][0][0]

//E_GAITS_RIPPLE
//UL [1][1][0][0][0][0]
//ML [0][0][0][0][1][1]
//BL [0][0][1][1][0][0]
//UR [0][0][0][1][1][0]
//MR [0][1][1][0][0][0]
//BR [1][0][0][0][0][1]


////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////

//struct of legs, 
typedef struct
{
	E_GAIT gaiting;
	E_WALK walking;
	Int16U walkingDelay;
	Int16U walkingAmplitude;
}SWalk;

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvWalkInit ( void ) ;
//Fonction de dispatching d'evenements
void SrvWalkUpdate (void) ;

SWalk* SrvWalkgetStruct (void) ;

Boolean SrvWalkSetAmplitude( Int8U amplitude );
Boolean SrvWalkSetWalk( E_WALK walk, uint16_t delay );
Boolean SrvWalkSetGait( E_GAIT gait, uint16_t delay );
#endif //SRVWALK_H_